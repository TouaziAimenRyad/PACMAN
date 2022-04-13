import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Random;

public class Comm_client implements Runnable {

    private Socket client;
    private OutputStream out;
    private InputStream in;
    private Player player=new Player();

    public Comm_client(Socket client)throws IOException {
        this.client = client;
        out = client.getOutputStream();
        in = client.getInputStream();
    }

    @Override
    public void run() {
        try {

            game_req();

            manage_requete();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void manage_requete() throws Exception {

        int inc = 0, nb;
        byte[] buff = new byte[5];

        boolean start=false;

        while(!start){
            
            inc=0;
            // read the first part of request
            while (inc < 5) {
                nb = in.read(buff, inc, 5 - inc);
                if(nb==-1){
                    return;
                }
                inc += nb;
            }

            switch (new String(buff)) {
                case External.UNREG:
                    unregister_game();
                    break;
                case External.SIZE_REQ:
                    size_reply();
                    break;
                case External.LIST_REQ:
                    list_of_players();
                    break;
                case External.GAME_REQ:
                    game_req();
                    break;
                case External.NEWLP:
                    create_new_game();
                    break;
                case External.START:
                    start_game();
                    break;
                case External.REGIS:
                    join_game();
                    break;
                default:
                    out.write(External.DUNNO, 0, External.DUNNO.length);
            }
        } 
        
        

    }

    private void game_req() throws Exception {

        byte[] buff = new byte[10];
        int offset = 0;

        // send games and list of ogams
        External.arraycopy(buff, offset, External.GAMES, 0, External.GAMES.length);
        offset += External.GAMES.length;
        External.arraycopy(buff, offset+1, External.ETOILES, 0, External.ETOILES.length);
        
        synchronized (Serveur.syn_nb_partie) {
            buff[offset] = Serveur.nb_partie;
            out.write(buff, 0, buff.length);
            // send games
            out.flush();
            buff= null;
            // check if they exist game
            if (Serveur.nb_partie > 0) {
                offset = 0;
                buff = new byte[12 * Serveur.nb_partie];
                // fill buffer with list of ogams
                for (Byte p : Serveur.list_parties_nc.keySet()) {
                    External.arraycopy(buff, offset, External.OGAMES, 0, External.OGAMES.length);
                    offset += External.OGAMES.length;
                    Partie partie=Serveur.list_parties_nc.get(p);
                    synchronized(partie){
                        buff[offset] = partie.getnumero();
                        offset++;
                        External.arraycopy(buff, offset, External.SEPARATE, 0, External.SEPARATE.length);
                        offset++;
                        buff[offset] = partie.getnbjoueur();
                    }
                    offset++;
                    External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
                    offset += External.ETOILES.length;
                }
            }

        }

        if(buff!=null){
            // send list of ogams
            out.write(buff, 0, buff.length);
            out.flush();
        }

    }

    private void create_new_game() throws Exception {

        byte[] buff = new byte[17];
        int inc = 0, nb;

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            inc += nb;
        }

        try {
            // set id player
            player.setid(new String(buff, 1, 8));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p<65536);
            player.setport(p);
            assert (new String(buff, 14, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
        } catch (Exception e) {
            // msg not in the right format
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }
        Random r = new Random();
        Byte n_game;
        Partie partie;
        synchronized(Serveur.syn_nb_partie){
            while (true) {
                n_game = (byte)(r.nextInt(255) + 1);
                if (!Serveur.list_parties_nc.containsKey(n_game))break;
            }
            partie=Serveur.add_new_game(n_game,player);
        
        }

        if(partie== null){
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        int offset=0;
        player.setgame(partie);
        External.arraycopy(buff, offset,External.REGOK,0,External.REGOK.length);
        offset+=External.REGOK.length;
        buff[offset]=n_game;
        External.arraycopy(buff, offset+1,External.ETOILES,0,External.ETOILES.length);
        out.write(buff, 0, 10);
        out.flush();
        
    }
    
    private void join_game() throws Exception{

        byte[] buff = new byte[19];
        int inc = 0, nb;
        byte n_game;

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            inc += nb;
        }
        
        try {
            // set id player
            player.setid(new String(buff, 1, 8));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p<65536);
            player.setport(p);
            n_game=buff[15];
            assert (new String(buff, 16, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
        } catch (Exception e) {
            // msg not in the right format
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        Partie partie=null;
        synchronized(Serveur.syn_nb_partie){
            if(!Serveur.list_parties_nc.containsKey(n_game)){
                out.write(External.REGNO, 0, External.REGNO.length);
                out.flush();
                return;
            }
            partie=Serveur.list_parties_nc.get(n_game);
        }

        if(!partie.add_player(player)){
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return; 
        }

        player.setgame(partie);
        int offset=0;
        External.arraycopy(buff, offset,External.REGOK,0,External.REGOK.length);
        offset+=External.REGOK.length;
        buff[offset]=n_game;
        External.arraycopy(buff, offset+1,External.ETOILES,0,External.ETOILES.length);
        out.write(buff, 0, 10);
        out.flush();
    }

    private void unregister_game() throws Exception{
        
        byte[] Buff=new byte[10];
        int inc=0,nb;

        while(inc<3){
            nb=in.read(Buff,inc, 3-inc);
            inc+=nb;
        }

        String st=new String(Buff,0,3);
        if (!st.equals(new String(External.ETOILES))){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        
        Partie game=player.getgame();
        // joueur non inscrit a une game
        if (game==null){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        byte num=game.getnumero();

        if (game.remove_player(player)){
            synchronized(Serveur.syn_nb_partie){
                Serveur.list_parties_nc.remove(num);
                Serveur.nb_partie--;   
            }
        }

        player.remove_from_game();
        int offset=0;
        External.arraycopy(Buff,offset,External.UNROK, 0,External.UNROK.length);
        offset+=External.UNROK.length;
        Buff[offset]=num;
        offset++;
        External.arraycopy(Buff,offset,External.ETOILES, 0,External.ETOILES.length);
        out.write(Buff, 0, 10);
        out.flush();

    }

    private void list_of_players()throws Exception{

        byte[] buff = new byte[5];
        int inc = 0, nb;

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            inc += nb;
        }

        byte n_game=buff[1];

        if (!new String(buff,2,3).equals(new String(External.ETOILES))){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie partie=null;

        synchronized(Serveur.syn_nb_partie){
            if(Serveur.list_parties_nc.containsKey(n_game)){
                partie=Serveur.list_parties_nc.get(n_game);
            }
        }

        if(partie==null){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        synchronized(partie){
            byte s=partie.getnbjoueur();
            
            if (s==0){
                out.write(External.DUNNO, 0, External.DUNNO.length);
                out.flush();
                return;
            }

            buff=new byte[12+17*s];
            int offset=0;
            External.arraycopy(buff,offset, External.LIST_REP,0, External.LIST_REP.length);
            offset+=External.LIST_REP.length;
            buff[offset]=n_game;
            buff[offset+1]=' ';
            buff[offset+2]=s;
            offset+=3;
            External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
            offset+=External.ETOILES.length;

            for(Player p:partie.joueurs.values()){
                External.arraycopy(buff, offset,External.PLAYER,0, External.PLAYER.length);
                offset+=External.PLAYER.length;
                External.arraycopy(buff, offset,p.getid().getBytes(),0, 8);
                offset+=8;
                External.arraycopy(buff, offset,External.ETOILES,0, 3);
                offset+=3;
            }

        }

        out.write(buff,0,buff.length);
        out.flush();

    }

    private void size_reply() throws Exception{
        byte[] buff = new byte[5];
        int inc = 0, nb;

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            inc += nb;
        }

        byte n_game=buff[1];

        if (!new String(buff,2,3).equals(new String(External.ETOILES))){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie partie=null;

        synchronized(Serveur.syn_nb_partie){
            if(Serveur.list_parties_nc.containsKey(n_game)){
                partie=Serveur.list_parties_nc.get(n_game);
            }
        }

        if(partie==null){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        short h,l;

        synchronized(partie){
            h=partie.get_hauteur();
            l=partie.get_largeur();
        }

        buff=new byte[16];
        int offset=0;
        External.arraycopy(buff,offset, External.SIZE_REP,0, External.SIZE_REP.length);
        offset+=External.SIZE_REP.length;
        buff[offset]=n_game;
        buff[offset+1]=' ';
        offset+=2;
        buff[offset] = (byte) h;
        buff[offset+1] = (byte) (h >> 8);
        offset+=2;
        buff[offset]=' ';
        buff[offset+1] = (byte) l;
        buff[offset+2] = (byte) (l >> 8);
        offset+=3;
        External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);

        out.write(buff,0,buff.length);
        out.flush();
    }

    private void start_game() throws Exception{
        byte buff[]=new byte[3];
        int inc=0;
        while(inc<3){
            inc+=in.read(buff, inc, 3-inc);
        }

    }

}