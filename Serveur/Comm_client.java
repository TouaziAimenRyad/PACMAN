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
            e.printStackTrace();
        }
    }

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
                    out.flush();
            }
        }

        if (start == null) {
            player.fermer_cnx();
            return;
        }

        game();

        if (start == null) {
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

    private void create_new_game() throws Exception {

        byte[] buff = new byte[17];
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

        //joueur deja inscrit
        if(player.getgame()!=null){
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
        }

        try {
            assert (new String(buff, 14, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p < 10000 && buff[0]==(byte)' ' && buff[9]==(byte)' ');
            // set id player
            player.setid(new String(buff, 1, 8));
            player.setport(p);
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
            partie = Serveur.add_new_game(n_game, player);

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
    
    // modifier
    // partie d'ajout du joueur dans la partie  dans le bloc syncroniz de list de partie
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

    //modifer 
    //comment lancer les thread endordmie dans la fonction qui suprimme le jouer
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

    private void start_game() throws Exception {
        byte[] buff = new byte[3];
        int inc = 0, nb;

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
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

    // verifier cas erreur
    private void game() throws Exception {
        int inc, nb;
        byte buff[] = new byte[5];

        welcome();
        send_positions();

        InputStream in=player.get_in();
        OutputStream out=player.get_out();

        Partie game = player.getgame();

        // on attend que tu le monde a recue les message de debut
        synchronized (game) {
            if (game.getnbstart() == 0){
                Serveur.list_parties_nc.remove(game.getnumero());
                Serveur.list_parties_c.put(game.getnumero(), game);
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

            switch (new String(buff)) {
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
                default:
                    out.write(External.DUNNO, 0, External.DUNNO.length);
                    out.flush();
                    break;
            }
        }
    }

    // traiter start = null ou false

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

        synchronized (game) {
            game.decnbrstart();
        }

    }

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

        Partie game = player.getgame();

        byte num = game.getnumero();

        if (game.remove_player(player)) {
            synchronized (Serveur.syn_partie_c) {
                Serveur.list_parties_c.remove(num);
                Serveur.set_socket_multi.remove(game.get_multicast());
            }
        }
        
        player.remove_from_game();

        out.write(External.GODEBYE, 0, External.GODEBYE.length);
        out.flush();
        start = false;
    }

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

        if(nb_font==0 && !player.get_game_finisher()){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }
        
        out.write(Buff, 0, Buff.length);
        out.flush();
        
    }

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

        if(nb_font==0 && !player.get_game_finisher()){
            out.write(External.GODEBYE, 0, External.GODEBYE.length);
            out.flush();
            start=false;
            return;
        }

        out.write(Buff, 0, Buff.length);
        out.flush();
    }

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
                }

            }

        }

        out.write(Buff, 0, Buff.length);
        out.flush();
    }

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

        while(inc<213){
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

        if(!new String(buff,inc-3,3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' ' || buff[9]!=(byte)' ' ){
            out.write(External.NSEND_REP,0,External.NSEND_REP.length);
            out.flush();
            return;
        }

        Partie game= player.getgame();

        synchronized(game){
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
            External.arraycopy(buff, offset, player.getid().getBytes(),0, player.getid().getBytes().length);
            offset+=player.getid().getBytes().length;
            buff2[offset]=(byte)' ';
            offset++;
            External.arraycopy(buff2, offset,buff, 10, inc-13);
            offset+=inc-13;
            External.arraycopy(buff2, offset,External.PLUSS, 0, External.PLUSS.length);
            DatagramPacket dp=new DatagramPacket(buff2, buff2.length,new InetSocketAddress(player_2.getAddress(), player_2.getPort()));
            
            DatagramSocket dso=new DatagramSocket();
            dso.send(dp);

            out.write(External.SEND_REP,0,External.SEND_REP.length);
            out.flush();

        }


    }

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

        if(!new String(buff,inc-3,3).equals(new String(External.ETOILES)) || buff[0]!=(byte)' '){
            out.write(External.NSEND_REP,0,External.NSEND_REP.length);
            out.flush();
            return;
        }

        out.write(External.MAIL_REP,0, External.MAIL_REP.length);
        out.flush();

        player.getgame().envoie_multi(buff,inc-4 ,player.getid());

    }

}