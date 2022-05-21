import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Random;

public class Partie {

    private byte numero;
    HashMap<String, Player> joueurs;
    private Byte nb_start = 0;
    private InetSocketAddress multicast;
    protected ArrayList<Thread_fantome> liste_thread_fontomes=new ArrayList<>();
    protected Labyrinthe labyrinthe;

    public Partie(byte numero, InetSocketAddress isa) {
        this.numero = numero;
        this.multicast = isa;
        this.joueurs = new HashMap<>();
    }

    // fonction qui initialise le labyrinthe
    void init_labyrinthe(boolean b){
        labyrinthe = new Labyrinthe(this,b);
    }
    
    byte getnumero() {
        return numero;
    }

    // fonction qui initialise les thread des fontomes
    void add_thread_fontome(Labyrinthe.Fantome f,short i,short j){
        liste_thread_fontomes.add(new Thread_fantome(f, i, j));
    }

    void set_nb_fontome(byte i){
        synchronized(labyrinthe){
            labyrinthe.set_nb_fontomes(i);
        }
    }

    byte get_nb_fontomes() {
        return labyrinthe.get_nb_fontomes();
    }

    Labyrinthe get_labyrinthe(){
        return labyrinthe;
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

    // fonction qui ajoute un joueur dans la partie
    synchronized boolean add_player(Player player) {
        if (joueurs.size() == 255 || joueurs.containsKey(player.getid()))
            return false;
            
        String key= player.getAddress().getHostAddress()+String.valueOf(player.getPort());       

        synchronized(Serveur.set_udp){
            if (Serveur.set_udp.contains(key)) return false;
            Serveur.set_udp.add(key);
        }
        
        joueurs.put(player.getid(), player);
        player.setgame(this);
        return true;
    }

    // fonction qui suprimme un joueur de la partie
    synchronized boolean remove_player(Player player) {
        joueurs.remove(player.getid());

        String key= player.getAddress().getHostAddress()+String.valueOf(player.getPort());       

        synchronized(Serveur.set_udp){
            Serveur.set_udp.remove(key);
        }
        

        if (joueurs.size() == 0)
            return true;

        if(nb_start==this.getnbjoueur()){
            Serveur.list_parties_nc.remove(this.getnumero());
            Serveur.list_parties_c.add(this);
            this.notifyAll();
        }

        return false;
    }

    void finish_thread(){
        liste_thread_fontomes=null;
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

    // fonction qui retourne l'addresse de multi cast en string qui respect le format du protocole
    String get_ip_mult() {
        String ip = multicast.getAddress().getHostAddress();
        for (int i = ip.length(); i < 15; i++)
            ip += "#";
        return ip;
    }

    // fonction qui retourne l'port de multi cast en string qui respect le format du protocole
    String get_port_mult() {
        return String.valueOf(multicast.getPort());
    }

    // fonction qui gere la requet du downmove du joueur
    void downmove( Short d, Player player) throws Exception{
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (i+1< get_hauteur() && matrice[i+1][j].route && d > 0) {
            i++;
            d--;
            labyrinthe.capture(player,i,j);
        }
        
        player.set_i(i);
    }

    // fonction qui gere la requet du leftmove du joueur
    void leftmove(Short d, Player player) throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (j > 0 && matrice[i][j-1].route && d > 0 ) {
            j--;
            d--;
            labyrinthe.capture(player,i,j);
        }
        player.set_j(j);
    }

    // fonction qui gere la requet du rightmove du joueur
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

    // fonction qui gere la requet du upmove du joueur
    void upmove(Short d, Player player)  throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (i>0 && matrice[i-1][j].route && d>0) {
            i--;
            d--;
            labyrinthe.capture(player, i,j);
        }
        
        player.set_i(i);
    }

    // verifier si la postion n'est pas un mur
    public boolean valid(int i, int j) {
        return labyrinthe.valid(i, j);
    }

    // fonction qui gere de l'envoie de multi multicast (mail)
    void envoie_multi(byte[] buff,int r,String id) throws Exception{

        byte[] buf=new byte[18+r];

        int offset=0;
        External.arraycopy(buf, offset,External.UDP_MAIL,0, External.UDP_MAIL.length);
        offset+=External.UDP_MAIL.length;
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

    // fonction qui gere l'envoie en multicast lorsque un joueur capteur un fontome
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

    // fonction qui gere le message de fin de partie (multicast / endga) 
    void endga() throws Exception{
        String id=null;
        Integer score=-1;
        for(Player p :joueurs.values()){
            synchronized(p){
                if( score< p.getscore()){
                    id=p.getid();
                    score=p.getscore();
                }
            }
        }

        byte Buff[]=new byte[22];
        External.arraycopy(Buff, 0, External.ENDGA, 0, External.ENDGA.length);
        int offset= External.ENDGA.length;
        External.arraycopy(Buff, offset, id.getBytes(), 0, id.getBytes().length);
        offset+=8;
        Buff[offset]=(byte)' ';
        offset++;
        String s=String.valueOf(score);
        while(s.length()<4){
            s="0"+s;
        }
        External.arraycopy(Buff, offset, s.getBytes(), 0, s.getBytes().length);
        offset+=4;
        External.arraycopy(Buff, offset, External.ETOILES, 0, External.ETOILES.length);

        DatagramSocket dso=new DatagramSocket();
        DatagramPacket dp=new DatagramPacket(Buff, Buff.length,multicast);

        synchronized(multicast){
            dso.send(dp);
        }
        dso.close();
    }

    // fonction qui lance les thread des fontomes
    void lunch_fontomes_threads(){
        for(Thread t :liste_thread_fontomes)t.start();
        
        System.out.print("-");
        for(int i=0;i<labyrinthe.get_matrice()[0].length;i++){
            System.out.print("--");
        }
        Labyrinthe.Cellul c;
        //affichage du labyrinthe
        for(Labyrinthe.Cellul[] l : labyrinthe.get_matrice()){
            System.out.println();
            System.out.print("|");
            for(int i=0;i<labyrinthe.get_matrice()[0].length;i++){
                c=l[i];
                if(i<labyrinthe.get_matrice()[0].length-1){
                    if(c.route)System.out.print(" |");
                    else System.out.print("*|");
                }else{
                    if(c.route)System.out.print(" ");
                    else System.out.print("*");
                }
            }
            System.out.print("|");
        }
        System.out.println();
        System.out.print("-");
        for(int i=0;i<labyrinthe.get_matrice()[0].length;i++){
            System.out.print("--");
        }
        System.out.println();

    }

    // fonction qui envoie la nouvelle postion du fantome lorsque il se deplace
    void send_pos_fon(Short i,Short j) throws Exception{
        DatagramSocket dso =new DatagramSocket();

        byte buff[]=new byte[16];

        External.arraycopy(buff, 0, External.GHOST, 0, External.GHOST.length);
        int offset=External.GHOST.length;

        String x=String.valueOf(i);
        while(x.length()<3){
            x="0"+x;
        }
        External.arraycopy(buff, offset, x.getBytes(), 0, x.getBytes().length);
        offset+=x.getBytes().length;
        buff[offset]=(byte) ' ';
        offset++;

        String y=String.valueOf(j);
        while(y.length()<3){
            y="0"+y;
        }
        External.arraycopy(buff, offset, y.getBytes(), 0, y.getBytes().length);
        offset+=y.getBytes().length;
        External.arraycopy(buff, offset, External.PLUSS, 0, External.PLUSS.length);

        DatagramPacket dp=new DatagramPacket(buff, buff.length,multicast);
        synchronized(multicast){
            dso.send(dp);
        }

        dso.close();

    }

    // class de thread de fantome
    class Thread_fantome extends Thread{
        Labyrinthe.Fantome f;
        short i,j;

        Thread_fantome(Labyrinthe.Fantome fa,short i,short j){
            f=fa;
            this.i=i;
            this.j=j;
        }

        @Override
        public void run(){
            Random r=new Random();
            while(labyrinthe.get_nb_fontomes()!=0 && !f.capturer){
                try {
                    sleep((r.nextInt(50)+20)*1000);
                    if (labyrinthe.get_nb_fontomes()==0 || f.capturer) break;
                    move();
                } catch (Exception e) {
                    e.printStackTrace();
                   return;
                }
            }
        }
        
        // fonction qui bouge le fantomme
        private void move() throws Exception{
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
                                send_pos_fon(i, j);
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
                                send_pos_fon(i, j);
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
                                send_pos_fon(i, j);
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
                                send_pos_fon(i, j);
                            }
                        }
                        else b=true;
                        break;
                }

            }
        } 

        // fonction qui change la postion  d'un fantomme
        protected void change_fontome(short i_prec,short j_prec,short new_i,short new_j){
            Labyrinthe.Cellul cel_prec=labyrinthe.get_matrice()[i_prec][j_prec];
            synchronized(cel_prec){
                cel_prec.ls_fantomes.remove(f);
                if(cel_prec.ls_fantomes.size()==0)
                    cel_prec.ls_fantomes=null;
            } 
            Labyrinthe.Cellul new_cel=labyrinthe.get_matrice()[new_i][new_j];
            synchronized(new_cel){
                if(new_cel.ls_fantomes==null)
                    new_cel.ls_fantomes=new ArrayList<>();
                new_cel.ls_fantomes.add(f);
            } 
        }
    }

}