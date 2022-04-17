import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Random;
import java.util.concurrent.ExecutionException;

public class Comm_client implements Runnable {

    private Socket client;
    private OutputStream out;
    private InputStream in;
    private Player player = new Player();
    private Boolean start = false;

    public Comm_client(Socket client) throws IOException {
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
            }
        }

        if (start == null) {
            in.close();
            out.close();
            client.close();
            return;
        }

        game();

    }

    private void game_req() throws Exception {

        byte[] buff = new byte[10];
        int offset = 0;

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

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        try {
            // set id player
            player.setid(new String(buff, 1, 8));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p < 65536);
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

    private void join_game() throws Exception {

        byte[] buff = new byte[19];
        int inc = 0, nb;
        byte n_game;

        // read the last part of request
        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        try {
            // set id player
            player.setid(new String(buff, 1, 8));
            // set port player
            int p = Integer.valueOf(new String(buff, 10, 4));
            assert (p > 1023 && p < 65536);
            player.setport(p);
            n_game = buff[15];
            assert (new String(buff, 16, 3).equals(new String(External.ETOILES, 0, External.ETOILES.length)));
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
        }

        if (!partie.add_player(player)) {
            out.write(External.REGNO, 0, External.REGNO.length);
            out.flush();
            return;
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

    private void unregister_game() throws Exception {

        byte[] Buff = new byte[10];
        int inc = 0, nb;

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
            }
        }
        Partie partie=player.getgame();
        synchronized(partie){
            if(partie.nb_start==partie.getnbjoueur())partie.notifyAll();
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

    // verifier le format de reception
    private void list_of_players() throws Exception {

        byte[] buff = new byte[5];
        int inc = 0, nb;

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

        if (!new String(buff, 2, 3).equals(new String(External.ETOILES))) {
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

    // verifier le format de recepetion
    private void size_reply() throws Exception {
        byte[] buff = new byte[5];
        int inc = 0, nb;

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

        if (!new String(buff, 2, 3).equals(new String(External.ETOILES))) {
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

    /// traiter si le joueur n'est pas inscrit
    private void start_game() throws Exception {
        byte[] buff = new byte[3];
        int inc = 0, nb;

        if (player.getgame() == null) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        while (inc < buff.length) {
            nb = in.read(buff, inc, buff.length - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }

        if (!new String(buff, 0, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }

        Partie game = player.getgame();

        if (game == null) {
            // joueur non inscrit
        } else {
            start = true;

            player.set_ij();
            synchronized (game) {
                game.incnbstart();
                if (game.getnbstart() == game.getnbjoueur())
                    game.notifyAll();
                else
                    game.wait();
            }
        }

    }

    // verifier cas erreur
    private void game() throws Exception {
        int inc, nb;
        byte buff[] = new byte[5];

        welcome();
        send_positions();

        Partie game = player.getgame();

        // on attend que tu le monde a recue les message de debut
        synchronized (game) {
            if (game.getnbstart() == 0)
                game.notifyAll();
            else
                game.wait();
        }

        while (start == true) {
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
                default:
                    break;
            }
        }
    }

    // traiter start = null ou false

    private void welcome() throws Exception {

        byte[] buff = new byte[39];
        int offset = 0;

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
        start = false;
    }

    private void send_move() throws Exception {
        byte[] Buff = new byte[16];
        int offset = 0;

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
        out.write(Buff, 0, Buff.length);
        out.flush();
    }

    private void send_move_score() throws Exception {
        byte[] Buff = new byte[21];
        int offset = 0;

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
        String st = player.getscore().toString();
        while (st.length() < 4)
            st = "0" + st;
        External.arraycopy(Buff, offset + 1, st.getBytes(), 0, 4);
        offset += 5;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        out.write(Buff, 0, Buff.length);
        out.flush();
    }

    private void downmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        Short d = Short.valueOf(new String(Buff,1,3));
        int s1 = player.getscore();
        Short nb_moves = player.getgame().downmove(Short.valueOf(player.get_string_i()),Short.valueOf(player.get_string_j()), d, player);
        player.set_i((short)(Short.valueOf(player.get_string_i())+ nb_moves));
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }

    private void leftmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        Short d = Short.valueOf(new String(Buff,1,3));
        int s1 = player.getscore();
        Short nb_moves = player.getgame().leftmove(Short.valueOf(player.get_string_i()),Short.valueOf(player.get_string_j()), d, player);
        player.set_j((short)(Short.valueOf(player.get_string_j()) - nb_moves));
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }

    private void rightmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        Short d = Short.valueOf(new String(Buff,1,3));
        int s1 = player.getscore();
        Short nb_moves = player.getgame().rightmove(Short.valueOf(player.get_string_i()),Short.valueOf(player.get_string_j()), d, player);
        player.set_j((short)(Short.valueOf(player.get_string_j()) + nb_moves));
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }

    private void upmove() throws Exception {
        byte[] Buff = new byte[7];
        int inc = 0, nb;

        while (inc < 7) {
            nb = in.read(Buff, inc, 7 - inc);
            if (nb == -1) {
                start = null;
                return;
            }
            inc += nb;
        }
        if (!new String(Buff, 4, 3).equals(new String(External.ETOILES))) {
            out.write(External.DUNNO, 0, External.DUNNO.length);
            out.flush();
            return;
        }
        Short d = Short.valueOf(new String(Buff,1,3));
        int s1 = player.getscore();
        Short nb_moves = player.getgame().upmove(Short.valueOf(player.get_string_i()),Short.valueOf(player.get_string_j()), d, player);
        player.set_i((short)(Short.valueOf(player.get_string_i()) - nb_moves));
        if (s1 != player.getscore()) {
            send_move_score();
        } else {
            send_move();
        }
    }

    private void Gamelist() throws Exception {
        byte[] Buff = new byte[3];
        int inc = 0, nb;

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

        byte nb_joueurs = player.getgame().getnbjoueur();
        Buff = new byte[10 + nb_joueurs * 30];
        int offset = 0;

        External.arraycopy(Buff, offset, External.GLIS_reply, 0, External.GLIS_reply.length);
        offset += External.GLIS_reply.length;
        Buff[offset] = nb_joueurs;
        offset++;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        offset += External.ETOILES.length;
        for (Player p : player.getgame().joueurs.values()) {
            External.arraycopy(Buff, offset, External.GPLYR, 0, External.GPLYR.length);
            offset += External.GPLYR.length;
            Buff[offset] = ' ';
            External.arraycopy(Buff, offset, p.getid().getBytes(), 0, 8);
            offset += 9;
            Buff[offset] = ' ';
            String x = p.get_string_i();
            String y = p.get_string_j();
            External.arraycopy(Buff, offset, x.getBytes(), 0, 3);
            offset += 3;
            Buff[offset] = ' ';
            External.arraycopy(Buff, offset + 1, y.getBytes(), 0, 3);
            offset += 4;
            Buff[offset] = ' ';
            String st = p.getscore().toString();
            while (st.length() < 4)
                st = "0" + st;
            External.arraycopy(Buff, offset + 1, st.getBytes(), 0, 4);
            offset += 5;
            External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);
        }
        out.write(Buff, 0, Buff.length);
        out.flush();
    }

}