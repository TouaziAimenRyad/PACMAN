import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.util.Random;

public class Comm_client implements Runnable {

    
    private Player player;
    private Boolean start = false;

    public Comm_client(Socket client) throws Exception {
        player=new Player(client);
    }

    @Override
    public void run() {
        try {

            game_req();

            manage_requete();

        } catch (Exception e) {
           try {
                e.printStackTrace();
                player.fermer_cnx();
            } catch (Exception e1) {
                e1.printStackTrace();
            }
        }
    }

    //fonction qui gere les requete du client
    private void manage_requete() throws Exception {

        int inc = 0, nb;
        byte[] buff = new byte[5];

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (start == false) {

            inc = 0;
            // read the first part of request
            while (inc < 5) {
                nb = in.read(buff, inc, 5 - inc);
                if (nb == -1) {
                    player.fermer_cnx();
                    return;
                }
                inc += nb;
            }
            
            System.out.println(new String(buff,0,5));
            switch (new String(buff,0,5)) {
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
                    inc = 0;
                    // read the first part of request
                    while (inc < 3) {
                        nb = in.read(buff, inc, 3 - inc);
                        if (nb == -1) {
                            player.fermer_cnx();
                            return;
                        }
                        inc += nb;
                    }
                    if(! new String(buff, 0, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length))){
                        out.write(External.DUNNO, 0, External.DUNNO.length);
                        out.flush();
                        break;
                    }
                    game_req();
                    break;
                case External.NEWPL:
                    create_new_game(false);
                    break;
                case External.NEWLP:
                    create_new_game(true);
                    break;
                case External.START:
                    start_game();
                    break;
                case External.REGIS:
                    join_game();
                    break;
                case External.GAMEC:
                    inc = 0;
                    // read the first part of request
                    while (inc < 3) {
                        nb = in.read(buff, inc, 3 - inc);
                        if (nb == -1) {
                            player.fermer_cnx();
                            return;
                        }
                        inc += nb;
                    }
                    if(! new String(buff, 0, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length))){
                        out.write(External.DUNNO, 0, External.DUNNO.length);
                        out.flush();
                        break;
                    }
                    gamec();
                    break;
                case External.LISTC:
                    listc();
                    break;
                case External.CHAID:
                    change_id();
                    break;
                case External.CHUDP:
                    change_udp();
                    break;
                default:
                    out.write(External.DUNNO, 0, External.DUNNO.length);
                    out.flush();
            }
        }

        if (start == null) {
            player.fermer_cnx();
            return;
        }

        game();

        if ( start != true) {
            player.fermer_cnx();
            return;
        }

    }
    
    //GAME REQUEST
    private void game_req() throws Exception {

        byte[] buff = new byte[10];
        int offset = 0;

        OutputStream out=player.get_out();

        // send games and list of ogams
        External.arraycopy(buff, offset, External.GAMES, 0, External.GAMES.length);
        offset += External.GAMES.length;
        External.arraycopy(buff, offset + 1, External.ETOILES, 0, External.ETOILES.length);

        synchronized (Serveur.syn_nb_partie) {
            buff[offset] = Serveur.nb_partie;
            out.write(buff, 0, buff.length);
            // send games
            out.flush();
            buff = null;
            // check if they exist game
            if (Serveur.nb_partie > 0) {
                offset = 0;
                buff = new byte[12 * Serveur.nb_partie];
                // fill buffer with list of ogams
                for (Byte p : Serveur.list_parties_nc.keySet()) {
                    External.arraycopy(buff, offset, External.OGAMES, 0, External.OGAMES.length);
                    offset += External.OGAMES.length;
                    Partie partie = Serveur.list_parties_nc.get(p);
                    synchronized (partie) {
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

        if (buff != null) {
            // send list of ogams
            out.write(buff, 0, buff.length);
            out.flush();
        }

    }

    //creer une nouvelle partie
    private void create_new_game(boolean b) throws Exception {

        byte[] buff = new byte[23];
        int inc = 0, nb,taille=17;
        short h=0,w=0;
        
        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        // read the last part of request
        
        if (b==true)
            taille=23;

        while (inc < taille) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        //joueur deja inscrit
        if(player.getgame()!=null){
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        try {
            assert (new String(buff, taille-3, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p < 10000 && buff[0]==(byte)' ' && buff[9]==(byte)' ');
            // set id player
            player.setid(new String(buff, 1, 8));
            player.setport(p);
            if(b){
                h=(short)((buff[15]<<8)+buff[16]);
                w=(short)((buff[18]<<8)+buff[19]);
            }
        } catch (Exception e) {
            // msg not in the right format
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }
        Random r = new Random();
        Byte n_game;
        Partie partie;
        synchronized (Serveur.syn_nb_partie) {
            while (true) {
                n_game = (byte) (r.nextInt(255) + 1);
                if (!Serveur.list_parties_nc.containsKey(n_game))
                    break;
            }
            partie = Serveur.add_new_game(n_game, player,b,h,w);

        }

        if (partie == null) {
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        int offset = 0;
        player.setgame(partie);
        External.arraycopy(buff, offset, External.REGOK, 0, External.REGOK.length);
        offset += External.REGOK.length;
        buff[offset] = n_game;
        External.arraycopy(buff, offset + 1, External.ETOILES, 0, External.ETOILES.length);
        out.write(buff, 0, 10);
        out.flush();

    }
    
    //rejoindre une partie existente
    private void join_game() throws Exception {

        byte[] buff = new byte[19];
        int inc = 0, nb;
        byte n_game;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        //joueur deja inscrit
        if(player.getgame()!=null){
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        try {
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p < 10000 && buff[0]==(byte)' ' && buff[9]==(byte)' ');
            assert (new String(buff, 16, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
            // set id player
            player.setid(new String(buff, 1, 8));
            player.setport(p);
            n_game = buff[15];
        } catch (Exception e) {
            // msg not in the right format
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        Partie partie = null;
        synchronized (Serveur.syn_nb_partie) {
            if (!Serveur.list_parties_nc.containsKey(n_game)) {
                out.write(External.REGNO, 0, External.REGNO.length);
                out.flush();
                return;
            }
            partie = Serveur.list_parties_nc.get(n_game);
            
            if (!partie.add_player(player)) {
                out.write(External.REGNO, 0, External.REGNO.length);
                out.flush();
                return;
            }

        }

        player.setgame(partie);
        int offset = 0;
        External.arraycopy(buff, offset, External.REGOK, 0, External.REGOK.length);
        offset += External.REGOK.length;
        buff[offset] = n_game;
        External.arraycopy(buff, offset + 1, External.ETOILES, 0, External.ETOILES.length);
        out.write(buff, 0, 10);
        out.flush();
    }

    // se desabonner d'une partie
    private void unregister_game() throws Exception {

        byte[] Buff = new byte[10];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 3) {
            nb = in.read(Buff, inc, 3 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        String st = new String(Buff, 0, 3);
        if (!st.equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie game = player.getgame();
        // joueur non inscrit a une game
        if (game == null) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        byte num = game.getnumero();

        if (game.remove_player(player)) {
            synchronized (Serveur.syn_nb_partie) {
                Serveur.list_parties_nc.remove(num);
                Serveur.nb_partie--;
                Serveur.set_socket_multi.remove(game.get_multicast());
            }
        }
        
        player.remove_from_game();
        int offset = 0;
        External.arraycopy(Buff, offset, External.UNROK, 0, External.UNROK.length);
        offset += External.UNROK.length;
        Buff[offset] = num;
        offset++;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        out.write(Buff, 0, 10);
        out.flush();

    }

    //la list des joueurs d'une partie 
    private void list_of_players() throws Exception {

        byte[] buff = new byte[5];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        byte n_game = buff[1];

        if (!new String(buff, 2, 3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' ' ) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie partie = null;

        synchronized (Serveur.syn_nb_partie) {
            if (Serveur.list_parties_nc.containsKey(n_game)) {
                partie = Serveur.list_parties_nc.get(n_game);
            }
        }

        if (partie == null) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        synchronized (partie) {
            byte s = partie.getnbjoueur();
            //verifier qu'il existe des joueur donc la partie existe
            if (s == 0) {
                out.write(External.DUNNO, 0, External.DUNNO.length);
                out.flush();
                return;
            }

            buff = new byte[12 + 17 * s];
            int offset = 0;
            External.arraycopy(buff, offset, External.LIST_REP, 0, External.LIST_REP.length);
            offset += External.LIST_REP.length;
            buff[offset] = n_game;
            buff[offset + 1] = ' ';
            buff[offset + 2] = s;
            offset += 3;
            External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
            offset += External.ETOILES.length;

            for (Player p : partie.joueurs.values()) {
                External.arraycopy(buff, offset, External.PLAYER, 0, External.PLAYER.length);
                offset += External.PLAYER.length;
                External.arraycopy(buff, offset, p.getid().getBytes(), 0, 8);
                offset += 8;
                External.arraycopy(buff, offset, External.ETOILES, 0, 3);
                offset += 3;
            }

        }

        out.write(buff, 0, buff.length);
        out.flush();

    }

    // requete de taille du l'abyrinthe
    private void size_reply() throws Exception {
        byte[] buff = new byte[5];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        byte n_game = buff[1];

        if (!new String(buff, 2, 3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' ' ) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie partie = null;

        synchronized (Serveur.syn_nb_partie) {
            if (Serveur.list_parties_nc.containsKey(n_game)) {
                partie = Serveur.list_parties_nc.get(n_game);
            }
        }

        if (partie == null) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        short h, l;

        //synchrinizer a cause de la desincription 
        synchronized (partie) {
            h = partie.get_hauteur();
            l = partie.get_largeur();
        }

        buff = new byte[16];
        int offset = 0;
        External.arraycopy(buff, offset, External.SIZE_REP, 0, External.SIZE_REP.length);
        offset += External.SIZE_REP.length;
        buff[offset] = n_game;
        buff[offset + 1] = ' ';
        offset += 2;
        buff[offset] = (byte) (h >> 8);
        buff[offset + 1] = (byte) h;
        offset += 2;
        buff[offset] = ' ';
        buff[offset + 1] = (byte) (l >> 8);
        buff[offset + 2] = (byte) l;
        offset += 3;
        External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);

        out.write(buff, 0, buff.length);
        out.flush();
    }

    //requete start
    private void start_game() throws Exception {
        byte[] buff = new byte[3];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start=null;
                return;
            }
            inc += nb;
        }

        Partie game = player.getgame();

        //non inscrit dans une partie ou format incorrect
        if (!new String(buff, 0, 3).equals(new String(External.ETOILES)) ||  game == null) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        
        start = true;

        player.set_ij();
        synchronized (game) {
            game.incnbstart();
            if (game.getnbstart() == game.getnbjoueur()){
                game.notifyAll();
            }else
                game.wait();
        }

    }

    // fonction qui gere le debut d'une partie
    private void game() throws Exception {
        int inc, nb;
        byte buff[] = new byte[5];

        welcome();
        send_positions();

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        Partie game = player.getgame();

        // on attend que tout le monde a recue les message de debut
        synchronized (game) {
            game.decnbrstart();
            if (game.getnbstart() == 0){
                synchronized(Serveur.syn_nb_partie){
                    Serveur.list_parties_nc.remove(game.getnumero());
                    Serveur.list_parties_c.add(game);
                    Serveur.nb_partie--;
                }
                game.notifyAll();
                game.lunch_fontomes_threads();
            }else
                game.wait();
        }

        while (start == true ) {
            inc = 0;
            // read the first part of request
            while (inc < 5) {
                nb = in.read(buff, inc, 5 - inc);
                if (nb == -1) {
                    start = null;
                    return;
                }
                inc += nb;
            }
            System.out.println(new String(buff,0,5));
            switch (new String(buff,0,5)) {
                case External.DOMOV:
                    downmove();
                    break;
                case External.LEMOV:
                    leftmove();
                    break;
                case External.RIMOV:
                    rightmove();
                    break;
                case External.UPMOV:
                    upmove();
                    break;
                case External.GLIS:
                    Gamelist();
                    break;
                case External.IQUIT:
                    quit();
                    break;
                case External.SEND_REQ:
                    Send();
                    break;
                case External.MAIL_REQ:
                    mail();
                    break;
                case External.PRINT :
                    print_lab();
                    break;
                case External.GHOPS :
                    ghost_cheat();
                    break;
                default:
                    out.write(External.DUNNO, 0, External.DUNNO.length);
                    out.flush();
                    break;
            }
        }
    }

    // fonction qui envoie welcomme au joueur 
    private void welcome() throws Exception {

        byte[] buff = new byte[39];
        int offset = 0;

        OutputStream out=player.get_out();

        External.arraycopy(buff, offset, External.WELCOM, 0, External.WELCOM.length);
        offset += External.WELCOM.length;
        Partie game = player.getgame();
        buff[offset] = game.getnumero();
        buff[offset + 1] = ' ';
        offset += 2;
        short h = game.get_hauteur();
        short l = game.get_largeur();
        buff[offset] = (byte) (h >> 8);
        buff[offset + 1] = (byte) h;
        offset += 2;
        buff[offset] = ' ';
        buff[offset + 1] = (byte) (l >> 8);
        buff[offset + 2] = (byte) l;
        offset += 3;
        buff[offset] = ' ';
        buff[offset + 1] = game.get_nb_fontomes();
        buff[offset + 2] = ' ';
        offset += 3;
        byte[] ip = game.get_ip_mult().getBytes();
        External.arraycopy(buff, offset, ip, 0, ip.length);
        offset += ip.length;
        buff[offset] = ' ';
        byte[] port = game.get_port_mult().getBytes();
        External.arraycopy(buff, offset + 1, port, 0, port.length);
        offset += 1 + port.length;
        External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);

        out.write(buff, 0, buff.length);
        out.flush();

    }
   
    // fonction qui envoie la postion des joueurs au debut d'une partie
    private void send_positions() throws Exception {

        byte[] buff = new byte[25];
        int offset = 0;

        OutputStream out=player.get_out();

        External.arraycopy(buff, offset, External.POSIT, 0, External.POSIT.length);
        offset += External.POSIT.length;
        byte[] id_buff = player.getid().getBytes();
        External.arraycopy(buff, offset, id_buff, 0, id_buff.length);
        offset += id_buff.length;
        buff[offset] = ' ';
        offset++;
        String x = player.get_string_i();
        String y = player.get_string_j();
        External.arraycopy(buff, offset, x.getBytes(), 0, 3);
        offset += 3;
        buff[offset] = ' ';
        External.arraycopy(buff, offset + 1, y.getBytes(), 0, 3);
        offset += 4;
        External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
        out.write(buff, 0, buff.length);
        out.flush();
    }

    // fonction de la requete quit
    private void quit() throws Exception {
        byte[] Buff = new byte[3];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 3) {
            nb = in.read(Buff, inc, 3 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        if (!new String(Buff, 0, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        out.write(External.GODEBYE, 0, External.GODEBYE.length);
        out.flush();

        player.fermer_cnx();

        start = false;
    }

    // fonction qui envoie le nouveau emplacement de joueur (lorsque le joueur se deplace et ne capture pas un fontome) 
    private void send_move() throws Exception {
        byte[] Buff = new byte[16];
        int offset = 0;

        OutputStream out=player.get_out();

        External.arraycopy(Buff, offset, External.MOVE, 0, External.MOVE.length);
        offset += External.MOVE.length;
        String x = player.get_string_i();
        String y = player.get_string_j();
        External.arraycopy(Buff, offset, x.getBytes(), 0, 3);
        offset += 3;
        Buff[offset] = ' ';
        External.arraycopy(Buff, offset + 1, y.getBytes(), 0, 3);
        offset += 4;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        
        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            if(!player.get_game_finisher()){
                out.write(External.GODEBYE, 0, External.GODEBYE.length);
                out.flush();
                start=false;
                return;
            }else
                player.set_game_finisher(false);
        }
        
        out.write(Buff, 0, Buff.length);
        out.flush();
        
    }

    // fonction qui evoie le nouveau emplacement de joueur avec nouveau score (lorsque le joueur se deplace et capture un fontome) 
    private void send_move_score() throws Exception {
        byte[] Buff = new byte[21];
        int offset = 0;

        OutputStream out=player.get_out();

        External.arraycopy(Buff, offset, External.MOVEF, 0, External.MOVEF.length);
        offset += External.MOVEF.length;
        String x = player.get_string_i();
        String y = player.get_string_j();
        External.arraycopy(Buff, offset, x.getBytes(), 0, 3);
        offset += 3;
        Buff[offset] = ' ';
        External.arraycopy(Buff, offset + 1, y.getBytes(), 0, 3);
        offset += 4;
        Buff[offset] = ' ';
        String st = player.getscore_string();
        External.arraycopy(Buff, offset + 1, st.getBytes(), 0, 4);
        offset += 5;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        
        
        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            if(!player.get_game_finisher()){
                out.write(External.GODEBYE, 0, External.GODEBYE.length);
                out.flush();
                start=false;
                return;
            }else
                player.set_game_finisher(false);
        }

        out.write(Buff, 0, Buff.length);
        out.flush();
    }

    // fonction qui gere la requete dowmove
    private void downmove() throws Exception {

        byte[] Buff = new byte[7];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES)) || Buff[0]!=(byte)' ') {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Short d;
        try {
            d = Short.valueOf(new String(Buff,1,3));
        } catch (Exception e) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        int s1 = player.getscore();
        synchronized(player){
            player.getgame().downmove(d, player);
        }
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }
    
    // fonction qui gere la requete leftmove
    private void leftmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES)) || Buff[0]!=(byte)' ') {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Short d;
        try {
            d = Short.valueOf(new String(Buff,1,3));
        } catch (Exception e) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        int s1 = player.getscore();
        synchronized(player){
            player.getgame().leftmove(d, player);
        }
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }

    }

    // fonction qui gere la requete rightmove
    private void rightmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES)) || Buff[0]!=(byte)' ') {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Short d;
        try {
            d = Short.valueOf(new String(Buff,1,3));
        } catch (Exception e) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        int s1 = player.getscore();
        synchronized(player){
            player.getgame().rightmove(d, player);
        }
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }

    // fonction qui gere la requete upmove
    private void upmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES)) || Buff[0]!=(byte)' ') {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Short d;
        try {
            d = Short.valueOf(new String(Buff,1,3));
        } catch (Exception e) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        int s1 = player.getscore();
        synchronized(player){
            player.getgame().upmove(d, player);
        }

        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }

    }

    // fonction qui gere la rquete gamelist
    private void Gamelist() throws Exception {
        byte[] Buff = new byte[3];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 3) {
            nb = in.read(Buff, inc, 3 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }

        if (!new String(Buff, 0, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie partie=player.getgame();

        synchronized(partie){
            byte nb_joueurs = partie.getnbjoueur();
            Buff = new byte[10 + nb_joueurs * 30];
            int offset = 0;
            
            External.arraycopy(Buff, offset, External.GLIS_reply, 0, External.GLIS_reply.length);
            offset += External.GLIS_reply.length;
            Buff[offset] = nb_joueurs;
            offset++;
            External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
            offset += External.ETOILES.length;

            for(Player p : partie.joueurs.values()){
                External.arraycopy(Buff, offset, External.GPLYR, 0, External.GPLYR.length);
                offset += External.GPLYR.length;
                External.arraycopy(Buff, offset, p.getid().getBytes(), 0, 8);
                offset += 8;
                Buff[offset] = ' ';
                offset++;

                synchronized(p){
                    String x = p.get_string_i();
                    String y = p.get_string_j();
                    External.arraycopy(Buff, offset, x.getBytes(), 0, 3);
                    offset += 3;
                    Buff[offset] = ' ';
                    offset++;
                    External.arraycopy(Buff, offset, y.getBytes(), 0, 3);
                    offset += 3;
                    Buff[offset] = ' ';
                    String st = p.getscore_string();
                    offset++;
                    External.arraycopy(Buff, offset, st.getBytes(), 0, 4);
                    offset += 4;
                    External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
                    offset+=3;
                }

            }

        }

        out.write(Buff, 0, Buff.length);
        out.flush();
    }

    // fonction qui gere l'envoie de message prive
    private void Send() throws Exception {
        byte buff[] =new byte[213];

        int inc=0;
        int r;
        
        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        String s="";

        while(inc<10){
            r=in.read(buff,inc,10-inc);
            if (r==-1) {
                start=null;
                return;
            }
            inc+=r;
        }

        while(inc<203){
            r=in.read(buff,inc,3);
            if(r==-1){
                start=null;
                return;
            }
            s+=new String(buff,inc,r);
            inc+=r;
            if(inc>12)
                if(s.substring(s.length()-3).equals("***")) break;
        }

        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }

        if(!new String(buff,inc-3,3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' ' || buff[9]!=(byte)' ' ){
            out.write(External.NSEND_REP,0,External.NSEND_REP.length);
            out.flush();
            return;
        }

        Partie game= player.getgame();

        Player player_2 = game.joueurs.get(new String(buff,1,8));
        if (player_2==null){
            out.write(External.NSEND_REP,0,External.NSEND_REP.length);
            out.flush();
            return;
        }


        byte buff2[] =new byte[5+inc];
        int offset=0;
        External.arraycopy(buff2, offset, External.UDP_SEND, 0, External.UDP_SEND.length);
        offset+=External.UDP_SEND.length;
        External.arraycopy(buff2, offset, player.getid().getBytes(),0, player.getid().getBytes().length);
        offset+=player.getid().getBytes().length;
        buff2[offset]=(byte)' ';
        offset++;
        External.arraycopy(buff2, offset,buff, 10, inc-13);
        offset+=inc-13;
        External.arraycopy(buff2, offset,External.PLUSS, 0, External.PLUSS.length);
        DatagramPacket dp=new DatagramPacket(buff2, buff2.length,new InetSocketAddress(player_2.getAddress(), player_2.getPort()));
        
        out.write(External.SEND_REP,0,External.SEND_REP.length);
        out.flush();
        DatagramSocket dso=new DatagramSocket();
        synchronized(player_2){
            dso.send(dp);
        }
        dso.close();

    }

    // fonction qui gere l'envoie de message global (ellle gere la requete mail)
    private void mail() throws Exception{
        byte buff[] =new byte[204];

        int inc=0;
        int r;
        
        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        r=in.read(buff,inc,1);
        if(r==-1){
            start=null;
            return;
        }
        inc++;

        String s="";
        while(inc<204){
            r=in.read(buff,inc,3);
            if(r==-1){
                start=null;
                return;
            }
            s+=new String(buff,inc,r);
            inc+=r;
            if(inc>3)
                if(s.substring(s.length()-3).equals("***")) break;
        }

        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }

        if(!new String(buff,inc-3,3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' '){
            out.write(External.DUNNO,0,External.DUNNO.length);
            out.flush();
            return;
        }

        out.write(External.MAIL_REP,0, External.MAIL_REP.length);
        out.flush();

        player.getgame().envoie_multi(buff,inc-4 ,player.getid());

    }

    // extension elle envoie les games qui sont entrain de se déroulé
    private void gamec() throws Exception{
        
        byte[] buff = new byte[13];
        int offset = 0;

        OutputStream out=player.get_out();

        // send games and list of ogams
        External.arraycopy(buff, offset, External.GAMES, 0, External.GAMES.length);
        offset += External.GAMES.length;
        External.arraycopy(buff, offset + 4, External.ETOILES, 0, External.ETOILES.length);

        synchronized (Serveur.syn_partie_c) {
            int t=Serveur.list_parties_c.size();
            buff[offset] = (byte) (t >> 24);
            buff[offset+1]=(byte) (t >> 16);
            buff[offset+2]=(byte) (t >> 8);
            buff[offset + 3] = (byte) t;
            offset+=4;
            out.write(buff, 0, buff.length);
            // send games
            out.flush();
            buff = null;
            // check if they exist game
            if (t > 0) {
                offset = 0;
                buff = new byte[12 * t];
                // fill buffer with list of ogams
                for (Partie partie : Serveur.list_parties_c) {
                    External.arraycopy(buff, offset, External.OGAMES, 0, External.OGAMES.length);
                    offset += External.OGAMES.length;
                    synchronized (partie) {
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

        if (buff != null) {
            // send list of ogams
            out.write(buff, 0, buff.length);
            out.flush();
        }

    }

    // extension qui envoie les parties et leurs joueurs des parties commencé (en donnant un numero de partie)
    private void listc()throws Exception{
        byte tmp[],buff[]=new byte[5],nb_joueurs;
        int i=0;

        int inc = 0, nb,offset=13;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < 5) {
            nb = in.read(buff, inc, 5 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        nb=buff[1];

        buff=new byte[13];

        synchronized(Serveur.syn_partie_c){

            for (Partie p : Serveur.list_parties_c){
                if(p.getnumero()==nb){
                    synchronized(p){
                        nb_joueurs=(byte)p.joueurs.size();
                        if(Byte.toUnsignedInt(nb_joueurs)>0){
                            i++;
                            tmp=new byte[buff.length];
                            External.arraycopy(tmp,0,buff, 0, buff.length);

                            buff = new byte[tmp.length+10 + nb_joueurs * 30];
                            External.arraycopy(buff,0,tmp, 0, tmp.length);
                            
                            External.arraycopy(buff, offset, External.GLIS_reply, 0, External.GLIS_reply.length);
                            offset += External.GLIS_reply.length;
                            buff[offset] = nb_joueurs;
                            offset++;
                            External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
                            offset += External.ETOILES.length;

                            for(Player pl : p.joueurs.values()){
                                External.arraycopy(buff, offset, External.GPLYR, 0, External.GPLYR.length);
                                offset += External.GPLYR.length;
                                External.arraycopy(buff, offset, pl.getid().getBytes(), 0, 8);
                                offset += 8;
                                buff[offset] = ' ';
                                offset++;

                                synchronized(pl){
                                    String x = pl.get_string_i();
                                    String y = pl.get_string_j();
                                    External.arraycopy(buff, offset, x.getBytes(), 0, 3);
                                    offset += 3;
                                    buff[offset] = ' ';
                                    offset++;
                                    External.arraycopy(buff, offset, y.getBytes(), 0, 3);
                                    offset += 3;
                                    buff[offset] = ' ';
                                    String st = pl.getscore_string();
                                    offset++;
                                    External.arraycopy(buff, offset, st.getBytes(), 0, 4);
                                    offset += 4;
                                    External.arraycopy(buff, offset, External.ETOILES, 0, External.ETOILES.length);
                                    offset+=3;
                                }

                            }

                        }
                    }

                }
            }

        }

        External.arraycopy(buff,0, External.GAMES, 0, External.GAMES.length);
        offset=6;
        buff[offset] = (byte) (i >> 24);
        buff[offset+1]=(byte) (i >> 16);
        buff[offset+2]=(byte) (i >> 8);
        buff[offset + 3] = (byte) i;
        offset+=4;
        External.arraycopy(buff,0, External.ETOILES, 0, External.ETOILES.length);
        out.write(buff, 0,buff.length);
        out.flush();

    }

    // extension qui gere la requete pour changer l'id d'un joueur
    private void change_id()throws Exception{
        byte buff[] =new byte[12];

        int inc=0;
        int r;
        
        InputStream in=player.get_in();
        OutputStream out=player.get_out();
    
        while(inc<12){
            r=in.read(buff, inc, 12-inc);
            if (r==-1) {
                start=null;
                return;
            }
            inc+=r;
        }

        Partie partie=player.getgame();
        String id=new String(buff, 1, 8);
        if( partie!=null){
            synchronized(partie){
                if (partie.joueurs.containsKey(id)){
                    External.arraycopy(buff, 0, External.CHANO,0, External.CHANO.length);
                    out.write(buff, 0, External.CHANO.length);;
                    return ;
                }
                partie.remove_player(player);
                player.setid(id);
                partie.add_player(player);
            }
        }

        External.arraycopy(buff, 0, External.CHAOK,0, External.CHAOK.length);
        out.write(buff, 0, External.CHAOK.length);;

    }

    // extension qui gere la requete pour changer l'id 
    private void change_udp() throws Exception{
        byte buff[] =new byte[8];

        int inc=0;
        int r;
        
        InputStream in=player.get_in();
        OutputStream out=player.get_out();
    
        while(inc<8){
            r=in.read(buff, inc, 8-inc);
            if (r==-1) {
                start=null;
                return;
            }
            inc+=r;
        }

        String key = player.getAddress().getHostAddress()+new String(buff, 1, 4);

        if(player.getgame()!=null){
            synchronized(Serveur.set_udp){
                Serveur.set_udp.add(key);
                Serveur.set_udp.remove(player.getAddress().getHostAddress()+player.getPort());
            }

            try {
                player.setport(Integer.valueOf(new String(buff, 1, 4)));

            } catch (Exception e) {
                External.arraycopy(buff, 0, External.CHANO,0, External.CHANO.length);
                out.write(buff, 0, External.CHANO.length);;
                return ;
            }

        }

        External.arraycopy(buff, 0, External.CHAOK,0, External.CHAOK.length);
        out.write(buff, 0, External.CHAOK.length);;

    }

    //extension de cheat mode
    private void print_lab() throws Exception{
        InputStream in=player.get_in();
        OutputStream out =player.get_out();

        byte buff[]=new byte[3];

        int inc = 0,nb;
        // read the first part of request
        while (inc < 3) {
            nb = in.read(buff, inc, 3 - inc);
            if (nb == -1) {
                start=null;
                return;
            }
            inc += nb;
        }

        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }
        
        if(! new String(buff, 0, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length))){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        byte mat[]=player.getgame().get_labyrinthe().getbyte_map();

        buff=new byte[12+mat.length];
        int offset=External.LABYR.length;
        External.arraycopy(buff, 0, External.LABYR, 0, External.LABYR.length);
        buff[offset]=(byte)' ';
        offset++;
        buff[offset]=(byte)(player.getgame().get_hauteur()>>8);
        offset++;
        buff[offset]=(byte)(player.getgame().get_hauteur());
        offset++;
        buff[offset]=(byte)' ';
        offset++;
        buff[offset]=(byte)(player.getgame().get_largeur()>>8);
        offset++;
        buff[offset]=(byte)(player.getgame().get_largeur());
        offset++;
        buff[offset]=(byte)' ';
        offset++;
        External.arraycopy(buff, offset, mat, 0, mat.length);
        
        out.write(buff, 0, buff.length);
        out.flush();

    }

    //extension de cheat mode
    private void ghost_cheat() throws Exception{
        InputStream in=player.get_in();
        OutputStream out =player.get_out();

        byte buff[]=new byte[3];

        int inc = 0,nb;
        // read the first part of request
        while (inc < 3) {
            nb = in.read(buff, inc, 3 - inc);
            if (nb == -1) {
                start=null;
                return;
            }
            inc += nb;
        }

        byte nb_font=player.getgame().get_nb_fontomes();

        if(nb_font==0){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }
        
        if(! new String(buff, 0, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length))){
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        buff =((Partie_extension)player.getgame()).get_list_thread_fantome();
        out.write(buff, 0, buff.length);
        out.flush();

    }

}
