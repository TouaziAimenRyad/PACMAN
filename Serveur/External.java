
public abstract class External {

    static byte[] GAMES = "GAMES ".getBytes();
    static byte[] ETOILES = "***".getBytes();
    static byte[] OGAMES = "OGAME ".getBytes();
    static byte[] SEPARATE = " ".getBytes();
    static byte[] DUNNO = "DUNNO***".getBytes();
    static byte[] UNROK = "UNROK ".getBytes();
    static byte[] SIZE_REP = "SIZE! ".getBytes();
    static byte[] LIST_REP = "LIST! ".getBytes();
    static byte[] REGNO = "REGNO***".getBytes();
    static byte[] REGOK = "REGOK ".getBytes();
    static byte[] PLAYER = "PLAYR ".getBytes();
    static byte[] WELCOM = "WELCO ".getBytes();
    static byte[] POSIT = "POSIT ".getBytes();
    static byte[] MOVE = "MOVE! ".getBytes();
    static byte[] MOVEF = "MOVEF ".getBytes();
    static byte[] GLIS_reply = "GLIS! ".getBytes();
    static byte[] GPLYR = "GPLYR ".getBytes();
    static byte[] GODEBYE = "GOBYE***".getBytes();
    static byte[] SEND_REP ="SEND!***".getBytes();
    static byte[] NSEND_REP ="NSEND***".getBytes();
    static byte[] UDP_SEND= "MESSP ".getBytes();
    static byte[] PLUSS= "+++".getBytes();
    static byte[] MAIL_REP= "MALL!***".getBytes();
    static byte[] UDP_MAIL= "MESSA ".getBytes();
    static byte[] CAPTURE_FAN="SCORE ".getBytes();
    static byte[] GHOST="GHOST ".getBytes();
    static byte[] ENDGA ="ENDGA ".getBytes();   

    static final String MAIL_REQ= "MALL?";
    static final String START = "START";
    static final String SEND_REQ="SEND?";
    static final String UPMOV = "UPMOV";
    static final String DOMOV = "DOMOV";
    static final String LEMOV = "LEMOV";
    static final String RIMOV = "RIMOV";
    static final String IQUIT = "IQUIT";
    static final String SIZE_REQ = "SIZE?";
    static final String LIST_REQ = "LIST?";
    static final String GAME_REQ = "GAME?";
    static final String UNREG = "UNREG";
    static final String NEWLP = "NEWPL";
    static final String REGIS = "REGIS";
    static final String GLIS = "GLIS?";

    static public void arraycopy(byte[] src, int offset, byte[] target, int offset_traget, int size) {
        for (int i = 0; i < size; i++) {
            src[offset + i] = target[offset_traget + i];
        }
    }

}