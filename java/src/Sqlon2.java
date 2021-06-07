import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class Sqlon2 implements Cloneable {
    public enum Type {
        NullT, BoolT, IntT, DoubleT, StringT, ListT, ObjectT, ArrayT
    }

    private Object data;
    private Type type;
    private boolean varLen = true;

    public Sqlon2() {
        data = null;
        type = Type.NullT;
    }

    public Sqlon2(boolean dat) {
        data = Boolean.valueOf(dat);
        type = Type.BoolT;
    }

    public Sqlon2(long dat) {
        data = Long.valueOf(dat);
        type = Type.IntT;
    }
    
    public Sqlon2(double dat) {
        data = Double.valueOf(dat);
        type = Type.DoubleT;
    }

    public Sqlon2(String dat, boolean varLen) {
        data = dat;
        type = Type.StringT;
        this.varLen = varLen;
    }

    public Sqlon2(List<Sqlon2> dat, boolean isArray) {
        data = new ArrayList<>(dat);
        type = isArray ? Type.ArrayT : Type.ListT;
    }

    public Sqlon2(Map<String, Sqlon2> dat) {
        data = new HashMap<>(dat);
        type = Type.ObjectT;
    }

    public boolean isNull() {
        return type.equals(Type.NullT);
    }

    public boolean getBoolean() {
        return (Boolean)data;
    }

    public long getInteger() {
        return (Long)data;
    }

    public double getDouble() {
        return (Double)data;
    }

    public String getString() {
        return (String)data;
    }

    public ArrayList<Sqlon2> getList() {
        assert data instanceof ArrayList;
        return (ArrayList<Sqlon2>)data;
    }

    public HashMap<String, Sqlon2> getObject() {
        return (HashMap<String, Sqlon2>)data;
    }

    @Override
    public Object clone() {
        switch (type) {
            case ArrayT:
                return new Sqlon2((ArrayList<Sqlon2>)getList().clone(), true);
            case BoolT:
                return new Sqlon2(getBoolean());
            case DoubleT:
                return new Sqlon2(getDouble());
            case IntT:
                return new Sqlon2(getInteger());
            case ListT:
                return new Sqlon2((ArrayList<Sqlon2>)getList().clone(), false);
            case ObjectT:
                return new Sqlon2((HashMap<String, Sqlon2>)getObject().clone());
            case StringT:
                return new Sqlon2(getString(), varLen);
            case NullT:
            default:
                return new Sqlon2();
        }
    }

    public String serialize() {
        StringBuilder sb = new StringBuilder();
        switch (type) {
            case ArrayT: {
                long size = getList().size();
                for (int i = 0; i < 8; i++) {
                    sb.append((byte)(size & (0xFF << (i << 3))) >> (i << 3));
                }

                for (Sqlon2 i : getList()) {
                    sb.append(i.serialize());
                }

                break;
            }
            case BoolT:
                sb.append(getBoolean() ? 'T' : 'F');
                break;
            case DoubleT: {
                long num = Double.doubleToRawLongBits(getDouble());
                for (int i = 0; i < 8; i++) {
                    sb.append((byte)((num & (0xFF << (i << 3))) >> (i << 3)));
                }
                break;
            }
            case IntT: {
                long num = getInteger();
                for (int i = 0; i < 8; i++) {
                    sb.append((byte)((num & (0xFF << (i << 3))) >> (i << 3)));
                }
                break;
            }
            case ListT: {
                for (Sqlon2 i : getList()) {
                    sb.append(i.serialize());
                }
                break;
            }
            case ObjectT:
                for (Map.Entry<String, Sqlon2> i : getObject().entrySet()) {
                    long size = i.getKey().length();
                    for (int j = 0; j < 8; j++) {
                        sb.append((byte)((size & (0xFF << (j << 3))) >> (j << 3)));
                    }
                    sb.append(i.getKey());
                    sb.append(i.getValue().serialize());
                }

                break;
            case StringT: {
                String str = getString();
                if (varLen) {
                    long size = str.length();
                    for (int i = 0; i < 8; i++) {
                        sb.append((byte)((size & (0xFF << (i << 3))) >> (i << 3)));
                    }
                }
                sb.append(str);
                break;
            }
            case NullT:
                sb.append('N');
                break;
            default:
        }
        return sb.toString();
    }

    public String describe() {
        StringBuilder sb = new StringBuilder();
        switch (type) {
            case ArrayT:
                if (getList().size() == 0) {
                    throw new IndexOutOfBoundsException();
                }
                sb.append('A');
                sb.append(getList().get(0).describe());
                break;
            case BoolT:
                sb.append('B');
                break;
            case DoubleT:
                sb.append('D');
                break;
            case IntT:
                sb.append('I');
                break;
            case ListT:
                sb.append('L').append(getList().size());
                for (Sqlon2 i : getList()) {
                    sb.append(i.describe());
                }
                break;
            case NullT:
                sb.append('N');
                break;
            case ObjectT:
                sb.append('O').append(getObject().size());
                for (Map.Entry<String, Sqlon2> i : getObject().entrySet()) {
                    sb.append(i.getValue().describe());
                }
                break;
            case StringT:
                if (varLen) {
                    sb.append('S');
                } else {
                    sb.append('S').append(getString().length());
                }
                break;
            default:
                break;
        }
        return sb.toString();
    }

    public static Sqlon2 deserialize(String data, String description) {
        int 
    }
}
