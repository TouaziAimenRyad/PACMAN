import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;

public class Serveur {

    static byte nb_partie = 0;
    static Object syn_nb_partie = new Object();
    static HashMap<Byte, Partie> list_parties_nc = new HashMap<>();
    static Object syn_partie_c = new Object();
    static ArrayList< Partie> list_parties_c = new ArrayList<>();
    static HashSet<InetSocketAddress> set_socket_multi=new HashSet<>();
    static HashSet<String> set_udp=new HashSet<>();

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

    // fonction qui genere une adresse de multi cast pour une partie 
    // on fait appeelle à elle dans la fontion add_new_game ci dessous
    private static InetSocketAddress generate_udp_multicast() {
        int min;
        int max;
        Short port;
        InetSocketAddress ret = null;

        while (true) {
            min = 1024;
            max = 9999;
            port = (short) Math.floor(Math.random() * (max - min + 1) + min);

            ret = new InetSocketAddress("226.194.197.139", port);
            if (!set_socket_multi.contains(ret)) {
                set_socket_multi.add(ret);
                break;
            }

        }

        return ret;
    }

    // fonction qui ajoute une nouvelle partie  
    static Partie add_new_game(byte n, Player player,boolean b,short h,short w) {
        if (nb_partie == 255)
        return null;
        // peutetre une erreur si on a plus de adr port de multicast (cas d'erreur est vraiment
        // trés petit seulement si on s'ature tout les port donc 9999-1023 partie)
        InetSocketAddress isa = generate_udp_multicast();
        Partie partie;
        if(b){
            partie = new Partie_extension(n, isa);
            ((Partie_extension) partie).init_labyrinthe(b,h,w);
        }
        else{
            partie = new Partie(n, isa);
            partie.init_labyrinthe(b);
        }
        if(!partie.add_player(player)){
            return null;
        }
        list_parties_nc.put(n, partie);
        nb_partie++;
        return partie;
    }

}