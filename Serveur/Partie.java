import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

public class Partie {

    private byte numero;
    private Labyrinthe labyrinthe = new Labyrinthe();
    HashMap<String, Player> joueurs;
    Byte nb_start = 0;
    private InetSocketAddress multicast;
    private ArrayList<Thread_fantome> liste_thread_fontomes=new ArrayList<>();

    public Partie(byte numero, InetSocketAddress isa) {
        this.numero = numero;
        this.multicast = isa;
        this.joueurs = new HashMap<>();
    }

    byte getnumero() {
        return numero;
    }

    byte get_nb_fontomes() {
        return labyrinthe.get_nb_fontomes();
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

    void decnbrstart() {
        nb_start--;
    }

    synchronized boolean add_player(Player player) {
        if (joueurs.size() == 255)
            return false;
        if (joueurs.containsKey(player.getid()))
            return false;
        joueurs.put(player.getid(), player);
        player.setgame(this);
        return true;
    }

    synchronized boolean remove_player(Player player) {
        joueurs.remove(player.getid());
        if (joueurs.size() == 0)
            return true;
        return false;
    }

    InetSocketAddress get_multicast(){
        return multicast;
    }

    short get_hauteur() {
        return labyrinthe.get_hauteur();
    }

    short get_largeur() {
        return labyrinthe.get_largeur();
    }

    String get_ip_mult() {
        String ip = multicast.getAddress().getHostAddress();
        for (int i = ip.length(); i < 15; i++)
            ip += "#";
        return ip;
    }

    String get_port_mult() {
        return String.valueOf(multicast.getPort());
    }

    void downmove( Short d, Player player) throws Exception{
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (i+1 < get_hauteur() && matrice[i+1][j].route && d > 0) {
            i++;
            d--;
            labyrinthe.capture(player,i,j);
        }
        
        player.set_i(i);
    }

    void leftmove(Short d, Player player) throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (j-1 < get_largeur() && matrice[i][j-1].route && d > 0 ) {
            j--;
            d--;
            labyrinthe.capture(player,i,j);
        }
        player.set_j(j);
    }

    void rightmove(Short d, Player player) throws Exception{
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (j+1 < get_largeur() && matrice[i][j+1].route && d > 0 ) {
            j++;
            d--;
            labyrinthe.capture(player,i,j);
        }
        player.set_j(j);
    }

    void upmove(Short d, Player player)  throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (i-1 >0 && matrice[i-1][j].route&& d > 0 && d>0) {
            i--;
            d--;
            labyrinthe.capture(player, i,j);
        }
        
        player.set_i(i);
    }

    public boolean valid(int i, int j) {
        return labyrinthe.valid(i, j);
    }

    void envoie_multi(byte[] buff,int r,String id) throws Exception{

        byte[] buf=new byte[18+r];

        int offset=0;
        External.arraycopy(buf, offset,External.UDP_MAIL,0, External.UDP_MAIL.length);
        offset+=External.UDP_MAIL.length;
        buf[offset]=(byte)' ';
        offset++;
        External.arraycopy(buf, offset,id.getBytes(),0, id.getBytes().length);
        offset+=id.getBytes().length;
        buf[offset]=(byte)' ';
        offset++;
        External.arraycopy(buf, offset,buff,1, r);
        offset+=r;
        External.arraycopy(buf, offset, External.PLUSS, 0, External.PLUSS.length);

        DatagramPacket dp=new DatagramPacket(buf, buf.length,multicast);
        DatagramSocket dso=new DatagramSocket();

        synchronized(multicast){
            dso.send(dp);
        }
        dso.close();
    }

    void envoie_capture(Player player,short i,short j) throws Exception{
        DatagramSocket dso=new DatagramSocket();
        byte[] buff =new byte[30];

        External.arraycopy(buff, 0, External.CAPTURE_FAN, 0, External.CAPTURE_FAN.length);
        int offset=External.CAPTURE_FAN.length;
        External.arraycopy(buff, offset, player.getid().getBytes(),0, player.getid().getBytes().length);
        offset+=player.getid().getBytes().length;
        buff[offset]=(byte)' ';
        offset++;
        External.arraycopy(buff, offset, player.getscore_string().getBytes(), 0, player.getscore_string().getBytes().length);
        offset+=player.getscore_string().getBytes().length;
        buff[offset]=(byte)' ';
        offset++;

        String ret = String.valueOf(i);
        for (int p = ret.length(); p < 3; p++) {
            ret = "0" + ret;
        }
        External.arraycopy(buff, offset, ret.getBytes(), 0, ret.getBytes().length);
        offset+=ret.getBytes().length;
        buff[offset]=(byte)' ';
        offset++;

        ret = String.valueOf(j);
        for (int p = ret.length(); p < 3; p++) {
            ret = "0" + ret;
        }
        External.arraycopy(buff, offset, ret.getBytes(), 0, ret.getBytes().length);
        offset+=ret.getBytes().length;

        External.arraycopy(buff, offset, External.PLUSS, 0, External.PLUSS.length);
        
        DatagramPacket dp=new DatagramPacket(buff, buff.length,multicast);

        synchronized(multicast){
            dso.send(dp);
        }
        dso.close();
    }

    void move_fantome(){
        Labyrinthe.Cellul[][] matrice=labyrinthe.get_matrice();
        ArrayList<Labyrinthe.Fantome> lf;
        for(short i=0;i<get_hauteur();i++){
            for(short j=0;j<get_hauteur();j++){
                lf=matrice[i][j].ls_fantomes;
                if(lf==null) 
                    continue;
                for(Labyrinthe.Fantome f :lf){
                    liste_thread_fontomes.add( new Thread_fantome(f, i, j));
                }
            }

        }
    }

    void lunch_fontomes_threads(){
        for(Thread t :liste_thread_fontomes)t.start();
        liste_thread_fontomes=null;
    }

    private class Thread_fantome extends Thread{
        Labyrinthe.Fantome f;
        short i,j;

        private Thread_fantome(Labyrinthe.Fantome fa,short i,short j){
            f=fa;
            this.i=i;
            this.j=j;
        }

        @Override
        public void run() {
            Random r=new Random();
            while(labyrinthe.get_nb_fontomes()!=0 && !f.capturer){
                try {
                    sleep(r.nextInt(20)+10);
                    move();
                } catch (InterruptedException e) {
                   return;
                }
            }
        }
        private void move(){
            Random r = new Random();
            boolean b;

            b = true;
            while (b && !f.capturer) {
                b=false;
                switch (r.nextInt(4)) {
                    case 0:
                        if (j + 1 < labyrinthe.get_largeur() && labyrinthe.get_matrice()[i][j+1].route) {
                            for(Player p :joueurs.values()){
                                if(p.get_i()==i && p.get_j()==j+1){
                                    b=true;
                                }
                            }
                            if(!b){
                                j++;
                                change_fontome(i, (short)(j-1), i, j);
                            }
                        }
                        else b=true;
                        break;
                    case 1:
                        if (j > 0 && labyrinthe.get_matrice()[i][j-1].route) {
                            for(Player p :joueurs.values()){
                                if(p.get_i()==i && p.get_j()==j-1){
                                    b=true;
                                }
                            }
                            if(!b){
                                j--;
                                change_fontome(i, (short)(j+1), i, j);
                            }
                        }
                        else b=true;
                        break;
                    case 2:
                        if (i > 0 && labyrinthe.get_matrice()[i-1][j].route) {
                            for(Player p :joueurs.values()){
                                if(p.get_i()==i-1 && p.get_j()==j){
                                    b=true;
                                }
                            }
                            if(!b){
                                i--;
                                change_fontome((short)(i+1), j, i, j);
                            }
                        }
                        else b=true;
                        break;
                    case 3:
                        if (i + 1 < labyrinthe.get_hauteur() && labyrinthe.get_matrice()[i+1][j].route) {
                            for(Player p :joueurs.values()){
                                if(p.get_i()==i+1 && p.get_j()==j){
                                    b=true;
                                }
                            }
                            if(!b){
                                i++;
                                change_fontome((short)(i-1), j, i, j);
                            }
                        }
                        else b=true;
                        break;
                }

            }
        } 

        private void change_fontome(short i_prec,short j_prec,short new_i,short new_j){
            Labyrinthe.Cellul cel_prec=labyrinthe.get_matrice()[i_prec][j_prec];
            synchronized(cel_prec){
                cel_prec.ls_fantomes.remove(f);
            } 
            Labyrinthe.Cellul new_cel=labyrinthe.get_matrice()[new_i][new_j];
            synchronized(new_cel){
                new_cel.ls_fantomes.add(f);
            } 
        }
    }

}