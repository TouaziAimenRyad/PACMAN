public class Player {

    private Byte game = null;
    private String id = null;
    private int port;

    void setid(String s) {
        id = s;
    }

    String getid() {
        return id;
    }

    void setport(int p) {
        port = p;
    }

    void setgame(byte g) {
        game=g;
    }

    Byte getgame(){
        return game;
    }

    void remove_from_game(){
        game=null;
        id=null;
    }

}