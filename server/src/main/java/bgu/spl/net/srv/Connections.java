package bgu.spl.net.srv;
import java.util.concurrent.atomic.AtomicInteger;


public interface Connections<T> {

    boolean send(AtomicInteger connectionId, T msg);

    void send(String channel, T msg);

    void disconnect(AtomicInteger connectionId);
}
