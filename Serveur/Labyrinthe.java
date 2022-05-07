import java.util.ArrayList;
import java.util.Random;

public class Labyrinthe {

    class Fantome{
        boolean capturer=false;
    }

    class Cellul{
        boolean route=false;
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

    public Labyrinthe(Partie p) {
        init((short) 50, (short) 50,p);
    }

    public Labyrinthe(short hauteur, short largeur,Partie p) {
        init(hauteur, largeur,p);
    }

    public void init(short hauteur, short largeur,Partie p) {
        this.hauteur = hauteur;
        this.largeur = largeur;
        this.nb_fontomes = (byte)(hauteur * largeur / 16);
        matrice = new Cellul[hauteur][largeur];

        for (int i = 0; i < hauteur; i++) {
            for (int j = 0; j < largeur; j++) {
                matrice[i][j]= new Cellul();
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

        int n = 0;
        int i, j;
        Fantome f;
        while (n < (nb_fontomes & 0xff)) {
            i = r.nextInt(this.get_hauteur());
            j = r.nextInt(this.get_largeur());
            if (matrice[i][j].route) {
                n++;
                if(matrice[i][j].ls_fantomes==null)
                    matrice[i][j].ls_fantomes=new ArrayList<>();
                f=new Fantome();
                matrice[i][j].ls_fantomes.add(f);
                p.add_thread_fontome(f, (short)i, (short)j);
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
        ArrayList<Fantome> rfs=new ArrayList<>();
        if(ls==null)return;

        for(Fantome f : ls){
            boolean b=false;

            synchronized(f){
                if (!f.capturer && ls.contains(f)){
                    f.capturer=true;
                    b=true;
                    rfs.add(f);
                }
            }

            if(b){
                player.incscore();
                desinc_nb_fontomes();
                player.getgame().envoie_capture(player,i,j);
                if (nb_fontomes==0) {
                    player.set_game_finisher(true);
                    player.getgame().endga();
                }
            }

        }

        ls.removeAll(rfs);


    }

    synchronized void desinc_nb_fontomes(){
        nb_fontomes--;
    }

    void set_nb_fontomes(byte i){
        nb_fontomes=i;
    }

}