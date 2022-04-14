import java.util.HashMap;

public class Partie {

    private byte numero;
    private Labyrinthe labyrinthe=new Labyrinthe();
    HashMap<String,Player> joueurs;
    Byte nb_start=0;


    public Partie(byte numero) {
        this.numero = numero;
        this.joueurs=new HashMap<>();
    }

    byte getnumero() {
        return numero;
    }

    byte get_nb_fontomes(){
        return nb_fontomes;
    }

    byte getnbjoueur() {
        return (byte) joueurs.size();
    }

    byte getnbstart() {
        return nb_start;
    }

    void incnbstart() {
        nb_start++;
    }

    synchronized boolean add_player(Player player){
        if(joueurs.size()==255)return false;
        if(joueurs.containsKey(player.getid())) return false;
        joueurs.put(player.getid(),player);
        player.setgame(this);
        return true;
    } 

    synchronized boolean remove_player(Player player){
        joueurs.remove(player.getid());
        if (joueurs.size()==0)return true;
        return false;
    }

    short get_hauteur(){
        return labyrinthe.get_hauteur();
    }

    short get_largeur(){
        return labyrinthe.get_largeur();
    }

    short downmove(Short i,Short j,Short d,Player player){
        Short n=d;
        char [][] matrice=labyrinthe.get_matrice();
        i++;
        while(matrice[i][j]!='*' && d>0 && i<get_hauteur()){
            if(matrice[i][j]=='@')player.incscore();
            i++;
            d--;
        }
        return n-d;
    }

    short leftmove(Short i,Short j,Short d,Player player){
        Short n=d;
        char [][] matrice=labyrinthe.get_matrice();
        j--;
        while(matrice[i][j]!='*' && d>0 && j>0){
            if(matrice[i][j]=='@')player.incscore();
            j--;
            d--;
        }
        return n-d;
    }

    short rightmove(Short i,Short j,Short d,Player player){
        Short n=d;
        char [][] matrice=labyrinthe.get_matrice();
        j++;
        while(matrice[i][j]!='*' && d>0 && j<get_largeur()){
            if(matrice[i][j]=='@')player.incscore();
            j++;
            d--;
        }
        return n-d;
    }

    short upmove(Short i,Short j,Short d,Player player){
        Short n=d;
        char [][] matrice=labyrinthe.get_matrice();
        i--;
        while(matrice[i][j]!='*' && d>0 && i<get_hauteur()){
            if(matrice[i][j]=='@')player.incscore();
            i--;
            d--;
        }
        return n-d;
    }
}