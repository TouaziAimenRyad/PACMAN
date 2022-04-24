import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Random;

public class Labyrinthe {

    class Fantome{
        boolean capturer=false;
    }

    class Cellul{
        boolean route=true;
        ArrayList<Fantome> ls_fantomes=null;
    }

    private short hauteur, largeur;
    private Cellul[][] matrice;
    private byte nb_fontomes;

    short get_hauteur() {
        return hauteur;
    }

    short get_largeur() {
        return largeur;
    }

    Cellul[][] get_matrice() {
        return matrice;
    }

    public Labyrinthe() {
        init((short) 50, (short) 50);
    }

    public Labyrinthe(short hauteur, short largeur) {
        init(hauteur, largeur);
    }

    public void init(short hauteur, short largeur) {
        this.hauteur = hauteur;
        this.largeur = largeur;
        this.nb_fontomes = (byte)(hauteur * largeur / 16);
        matrice = new Cellul[hauteur][largeur];

        for (int i = 0; i < hauteur; i++) {
            for (int j = 0; j < largeur; j++) {
                matrice[i][j].route= false;
            }
        }

        Random r = new Random();
        int l = r.nextInt(hauteur), c = r.nextInt(largeur);

        matrice[l][c].route= true;
        boolean b;
        int densité = 0;

        while (densité < hauteur * largeur * 0.5) {
            b = true;
            while (b) {

                switch (r.nextInt(4)) {
                    case 0:
                        if (c + 1 < largeur) {
                            c++;
                            b = false;
                        }
                        break;
                    case 1:
                        if (c > 0) {
                            c--;
                            b = false;
                        }
                        break;
                    case 2:
                        if (l > 0) {
                            l--;
                            b = false;
                        }
                        break;
                    case 3:
                        if (l + 1 < hauteur) {
                            l++;
                            b = false;
                        }
                        break;
                }

            }
            if (!matrice[l][c].route) {
                densité++;
                matrice[l][c].route=true;
            }
        }

        byte n = 0;
        int i, j;
        while (n < nb_fontomes) {
            i = r.nextInt(this.get_hauteur());
            j = r.nextInt(this.get_largeur());
            if (matrice[i][j].route) {
                n++;
                if(matrice[i][j].ls_fantomes==null)
                    matrice[i][j].ls_fantomes=new ArrayList<>();
                matrice[i][j].ls_fantomes.add(new Fantome());
            }
        }

    }

    public boolean valid(int i, int j) {
        if (i >= hauteur || i < 0 || j < 0 || j >= largeur || !matrice[i][j].route)
            return false;
        return true;
    }

    byte get_nb_fontomes() {
        return nb_fontomes;
    }

    void capture(Player player,short i,short j) throws Exception{
        
        ArrayList<Fantome> ls=matrice[i][j].ls_fantomes;
        
        for(Fantome f : ls){
            boolean b=false;

            synchronized(f){
                if (!f.capturer && ls.contains(f)){
                    f.capturer=true;
                    b=true;
                    ls.remove(f);
                }
            }

            if(b){
                player.getgame().envoie_capture(player,i,j);
                player.incscore();
                desinc_nb_fontomes();
                if (nb_fontomes==0) 
                    player.set_game_finisher(true);
            }

        }

    }

    synchronized void desinc_nb_fontomes(){
        nb_fontomes--;
    }

    public static void main(String[] args) {
        Labyrinthe l = new Labyrinthe((short) 100, (short) 100);
        for (int i = 0; i < l.hauteur; i++) {
            for (int j = 0; j < l.largeur; j++) {
                if (l.matrice[i][j].route)
                    System.out.print( "  ");
                else 
                System.out.print("* ");
            }
            System.out.println();
        }
    }

}