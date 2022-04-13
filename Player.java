public class Player {

    private String id = null;
    private int port;
    private Partie partie=null;

    void setid(String s) {
        id = s;
    }

    String getid() {
        return id;
    }

    void setport(int p) {
        port = p;
    }

    Partie getgame(){
        return partie;
    }

    void setgame(Partie p) {
        partie=p;
    }

    void remove_from_game(){
        partie=null;
    }

}