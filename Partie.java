import java.util.HashMap;

public class Partie {

    private byte numero;
    private boolean start = false;
    HashMap<String,Player> joueurs;

    public Partie(byte numero) {
        this.numero = numero;
        this.joueurs=new HashMap<>();
    }

    byte getnumero() {
        return numero;
    }

    byte getnbjoueur() {
        return (byte) joueurs.size();
    }

    synchronized boolean add_player(Player player){
        if(joueurs.size()==255)return false;
        if(joueurs.containsKey(player.getid())) return false;
        joueurs.put(player.getid(),player);
        return true;
    } 

    synchronized boolean remove_player(Player player){
        joueurs.remove(player.getid());
        if (joueurs.size()==0)return true;
        return false;
    }

}