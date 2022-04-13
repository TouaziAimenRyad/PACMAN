import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

public class Serveur {

    static byte nb_partie = 0;
    static Object syn_nb_partie = new Object();
    static HashMap<Byte, Partie> list_parties_nc= new HashMap<>();

    public static void main(String[] args) {

        assert (args.length == 1);
        int n_port = Integer.valueOf(args[0]);
        assert (n_port > 1023);

        try {

            ServerSocket socket = new ServerSocket(n_port);

            while (true) {
                Socket client = socket.accept();
                try{
                    new Thread(new Comm_client(client)).start();
                }catch(IOException e){}
            }

        } catch (Exception e) {
            e.printStackTrace();
        }

    }

    static Partie add_new_game(byte n,Player player){
        if(nb_partie==255)return null;
        Partie partie =new Partie(n);
        partie.add_player(player);
        list_parties_nc.put(n,partie);
        nb_partie++;
        return partie;
    }

}