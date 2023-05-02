package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.srv.ConnectionsImpl;
import java.util.concurrent.atomic.AtomicInteger;

public interface StompMessagingProtocol<Frame> extends MessagingProtocol<Frame> {
	/**
	 * Used to initiate the current client protocol with it's personal connection ID and the connections implementation
	**/
    void start(AtomicInteger connectionId, ConnectionsImpl<Frame> connections);
    
    Frame process(Frame message);
	
	/**
     * @return true if the connection should be terminated
     */
    boolean shouldTerminate();
}
