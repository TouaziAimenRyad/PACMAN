import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Random;

public class Player {

    private Socket client;
    private OutputStream out;
    private InputStream in;
    private String id = null;
    private int port;
    private Partie partie = null;
    private InetAddress address=null;
    private Short i, j;
    private Integer score = 0;
    private boolean game_finisher=false;

    public Player (Socket c) throws Exception{
        client=c;
        out=c.getOutputStream();
        in=c.getInputStream();
        address=client.getInetAddress();
    }

    InetAddress getAddress() {
        return address;
    }

    public int getPort() {
        return port;
    }

    void set_adress(InetAddress s){
        address=s;
    }

    void setid(String s) {
        id = s;
    }

    String getid() {
        return id;
    }

    void setport(int p) {
        port = p;
    }

    Partie getgame() {
        return partie;
    }

    void setgame(Partie p) {
        partie = p;
    }

    //enleve le joueur d'une partie
    void remove_from_game() {
        partie = null;
        id=null;
    }

    //initialise le joeur dans la map
    void set_ij() {
        Random r = new Random();
        i = (short)r.nextInt(partie.get_hauteur());
        j = (short)r.nextInt(partie.get_largeur());
        while (!partie.valid(i, j)) {
            i = (short)r.nextInt(partie.get_hauteur());
            j = (short)r.nextInt(partie.get_largeur());
        }
    }

    // recuperer la postion x du joueur en format string qui respect le format 
    String get_string_i() {
        String ret = String.valueOf(i);
        for (int p = ret.length(); p < 3; p++) {
            ret = "0" + ret;
        }
        return ret;
    }

    // recuperer la postion y du joueur en format string qui respect le format 
    String get_string_j() {
        String ret = String.valueOf(j);
        for (int p = ret.length(); p < 3; p++) {
            ret = "0" + ret;
        }
        return ret;
    }

    void set_i(Short i) {
        this.i = i;
    }

    void set_j(Short j) {
        this.j = j;
    }

    void incscore() {
        score++;
    }

    void incremente_i(){
        i++;
    }
    
    void des_incremente_i(){
        i--;
    }
    
    void incremente_j(){
        j++;
    }
    
    void des_incremente_j(){
        j--;
    }

    short get_i(){
        return i;
    }
    
    short get_j(){
        return j;
    }

    // recuperer la le score du joueur en format string qui respect le format 
    String getscore_string() {
        String r=score.toString();
        for(int i=r.length();i<4;i++)
            r="0"+r;
        return r;
    }

    int getscore(){
        return score;
    }

    // fonctioon qui gere la fermeteure de la connexion avec le joueur
    void fermer_cnx() throws Exception{
        in.close();
        out.close();
        client.close();
        if(partie!=null){
            if(partie.remove_player(this)){
                synchronized(Serveur.syn_nb_partie){
                    if(Serveur.list_parties_nc.get(partie.getnumero())!=null){
                        Serveur.list_parties_nc.remove(partie.getnumero());
                        Serveur.set_socket_multi.remove(partie.get_multicast());
                    }
                }

                synchronized(Serveur.syn_partie_c){
                    if(Serveur.list_parties_c.contains(partie)){
                        Serveur.list_parties_c.remove(partie);
                        Serveur.set_socket_multi.remove(partie.get_multicast());
                        partie.set_nb_fontome((byte)0);
                    }
                }
                
            }
        }

        if(partie!=null){
            String key=address.getHostAddress()+port;

            synchronized(Serveur.set_udp){
                Serveur.set_udp.remove(key);
            }
        }

    }

    void set_game_finisher(boolean b){
        game_finisher=b;
    }

    boolean get_game_finisher(){
        return game_finisher;
    }

    InputStream get_in(){
        return in;
    }

    OutputStream get_out(){
        return out;
    }

}
