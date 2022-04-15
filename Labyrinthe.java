import java.util.Random;

public class Labyrinthe {

    private short hauteur, largeur;
    private char[][] matrice;
    private byte nb_fontomes;

    short get_hauteur() {
        return hauteur;
    }

    short get_largeur() {
        return largeur;
    }

    char[][] get_matrice() {
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
        this.nb_fontomes = hauteur * largeur / 16;
        matrice = new char[hauteur][largeur];

        for (int i = 0; i < hauteur; i++) {
            for (int j = 0; j < largeur; j++) {
                matrice[i][j] = '*';
            }
        }

        Random r = new Random();
        int l = r.nextInt(hauteur), c = r.nextInt(largeur);

        matrice[l][c] = ' ';
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
            if (matrice[l][c] == '*') {
                densité++;
                matrice[l][c] = ' ';
            }
        }
        byte n = 0;
        int i, j;
        while (n < nb_fontomes) {
            i = r.nextInt(L.get_hauteur());
            j = r.nextInt(L.get_largeur());
            if (matrice[i][j] == ' ') {
                matrice[i][j] = '@';
                n++;
            }
        }

    }

    public boolean valid(int i, int j) {
        if (i >= hauteur || i < 0 || j < 0 || j >= largeur || matrice[i][j] == "*")
            return false;
        return true;
    }

    byte get_nb_fontomes() {
        return nb_fontomes;
    }

    public static void main(String[] args) {
        Labyrinthe l = new Labyrinthe((short) 100, (short) 100);
        for (int i = 0; i < l.hauteur; i++) {
            for (int j = 0; j < l.largeur; j++) {
                System.out.print(l.matrice[i][j] + " ");
            }
            System.out.println();
        }
    }

}