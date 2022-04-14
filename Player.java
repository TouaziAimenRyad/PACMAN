public class Player {

    private String id = null;
    private int port;
    private Partie partie=null;
    private Short i,j;
    private Integer score=0;

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

    void set_ij(){
        Random r=new Random();
        Labyrinthe L=partie.Labyrinthe;
        i=r.nextInt(L.get_hauteur());
        j=r.nextInt(L.get_largeur());
        while(!L.valid(i,j)){
            i=r.nextInt(L.get_hauteur());
            j=r.nextInt(L.get_largeur());
        }
    }

    Short get_i(){
        return  i;
    }

    Short get_j(){
        return  j;
    }

    void set_i(Short i){
        this.i=i;
    }

    void set_j(Short j){
        this.j=j;
    }

    void incscore(){
        score++;
    }

    Integer getscore(){
        return score;
    }
}