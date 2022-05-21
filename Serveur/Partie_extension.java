import java.net.InetSocketAddress;
import java.util.Random;

public class Partie_extension extends Partie{
    
    public Partie_extension(byte numero, InetSocketAddress isa){
        super(numero, isa);
    }

    void init_labyrinthe(boolean b,short h,short w){
        labyrinthe = new Labyrinthe(h,w,this,b);
    }

    // redifinotion de la fonction qui initialise les thread des fontomes
    void add_thread_fontome(Labyrinthe.Fantome f,short i,short j){
        liste_thread_fontomes.add(new Thread_fantome_extension(f, i, j));
    }

    // redefintion de la fonction qui gere le downmove 
    void downmove( Short d, Player player) throws Exception{
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (matrice[((i+1)%get_hauteur())][j].route && d > 0) {
            i=(short)((i+1)%get_hauteur());
            d--;
            labyrinthe.capture(player,i,j);
        }
        
        player.set_i(i);
    }
    
    // redefintion de la fonction qui gere le leftmove 
    void leftmove(Short d, Player player) throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (matrice[i][(j-1+get_largeur())%get_largeur()].route && d > 0 ) {
            j=(short)((j-1+get_largeur())%get_largeur());
            d--;
            labyrinthe.capture(player,i,j);
        }
        player.set_j(j);
    }

    // redefintion de la fonction qui gere le rightmove 
    void rightmove(Short d, Player player) throws Exception{
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while (matrice[i][(j+1)%get_largeur()].route && d > 0 ) {
            j=(short)((j+1)%get_largeur());
            d--;
            labyrinthe.capture(player,i,j);
        }
        player.set_j(j);
    }

    // redefintion de la fonction qui gere le upmove 
    void upmove(Short d, Player player)  throws Exception {
        Labyrinthe.Cellul[][] matrice = labyrinthe.get_matrice();
        short i=player.get_i();
        short j=player.get_j();

        while ( matrice[(i-1+get_hauteur())%get_hauteur()][j].route && d>0) {
            i=(short)((i-1+get_hauteur())%get_hauteur());
            d--;
            labyrinthe.capture(player, i,j);
        }
        
        player.set_i(i);
    }

    // fonction des thread de fantome en mode extension
    class Thread_fantome_extension extends Partie.Thread_fantome{

        Thread_fantome_extension(Labyrinthe.Fantome fa,short i,short j){
            super(fa, i, j);
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

        // redifition de la fonction move
        private void move() throws Exception{
            Random r = new Random();
            boolean b;
            int pi,pj;
            b = true;
            while (b && !f.capturer) {
                b=false;
                pi=r.nextInt(get_hauteur());
                pj=r.nextInt(get_largeur());
                if(labyrinthe.get_matrice()[pi][pj].route){
                    for(Player p :joueurs.values()){
                        if(p.get_i()==pi && p.get_j()==pj){
                            b=true;
                        }
                    }
                    if(!b){
                        change_fontome(i, j, (short)(pi), (short)(pj));
                        i=(short)pi;
                        j=(short)pj;
                        send_pos_fon(i, j);
                    }
                }else b=true;
                
            }
        } 

    }

    //extension retourne les fantome non capture avec leur postion
    byte[] get_list_thread_fantome(){
        String fan="";
        byte cmpt=0;
        String y;

        for (Partie.Thread_fantome th : liste_thread_fontomes){
                if(!th.f.capturer){
                    y=String.valueOf(th.i);
                    while(y.length()<3){
                        y="0"+y;
                    }
                    fan+="CHOST "+y+" ";
                    y=String.valueOf(th.j);
                    while(y.length()<3){
                        y="0"+y;
                    }
                    fan+=y+"***";
                    cmpt++;
                }
        }

        byte buff[]=new byte[10+cmpt*16];
        External.arraycopy(buff, 0, "GHORE ".getBytes(), 0, "GHORE ".getBytes().length);
        int offset="GHORE ".getBytes().length;
        buff[offset]=cmpt;
        offset++;
        fan="***"+fan;
        External.arraycopy(buff, offset, fan.getBytes(),0, fan.getBytes().length);
    
        return buff;
    }  


}
