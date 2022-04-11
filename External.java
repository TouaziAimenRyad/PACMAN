
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
    static byte[] PLAYER = "PLAYER ".getBytes();

    static final String START = "START";
    static final String SIZE_REQ = "SIZE?";
    static final String LIST_REQ = "LIST?";
    static final String GAME_REQ = "GAME?";
    static final String UNREG = "UNREG";
    static final String NEWLP = "NEWPL";
    static final String REGIS = "REGIS";

    static public void arraycopy(byte[] src, int offset, byte[] target, int offset_traget, int size) {
        for (int i = 0; i < size; i++) {
            src[offset + i] = target[offset_traget + i];
        }
    }

}