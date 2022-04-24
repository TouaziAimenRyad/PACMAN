import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.HashSet;

public class Serveur {

    static byte nb_partie = 0;
    static Object syn_nb_partie = new Object();
    static HashMap<Byte, Partie> list_parties_nc = new HashMap<>();
    static Object syn_partie_c = new Object();
    static HashMap<Byte, Partie> list_parties_c = new HashMap<>();
    static HashSet<InetSocketAddress> set_socket_multi=new HashSet<>();

    public static void main(String[] args) {

        assert (args.length == 1);
        int n_port = Integer.valueOf(args[0]);
        assert (n_port > 1023);

        try {

            ServerSocket socket = new ServerSocket(n_port);

            while (true) {
                Socket client = socket.accept();
                try {
                    new Thread(new Comm_client(client)).start();
                } catch (IOException e) {
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    private static InetSocketAddress generate_udp_multicast() {
        int min;
        int max;
        Short port;
        InetSocketAddress ret = null;

        while (true) {
            min = 224;
            max = 239;
            int c = (int) Math.floor(Math.random() * (max - min + 1) + min);
            min = 0;
            max = 255;
            int c1 = (int) Math.floor(Math.random() * (max - min + 1) + min);
            min = 0;
            max = 255;
            int c2 = (int) Math.floor(Math.random() * (max - min + 1) + min);
            min = 1;
            max = 254;
            int c3 = (int) Math.floor(Math.random() * (max - min + 1) + min);

            String ip = String.valueOf(c) + "." + String.valueOf(c1) + "." + String.valueOf(c2) + "."
                    + String.valueOf(c3);

            min = 1024;
            max = 9999;
            port = (short) Math.floor(Math.random() * (max - min + 1) + min);

            ret = new InetSocketAddress(ip, port);
            if (!set_socket_multi.contains(ret)) {
                set_socket_multi.add(ret);
                break;
            }

        }

        return ret;
    }

    static Partie add_new_game(byte n, Player player) {
        if (nb_partie == 255)
            return null;
        // peutetre une erreur si on a plus de adr port de multicast
        InetSocketAddress isa = generate_udp_multicast();
        Partie partie = new Partie(n, isa);
        partie.add_player(player);
        list_parties_nc.put(n, partie);
        nb_partie++;
        return partie;
    }

}