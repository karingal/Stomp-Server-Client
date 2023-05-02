package bgu.spl.net.impl.stomp;

import java.util.Queue;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicInteger;


public class Topic {
    public String topicName;
    public Queue<AtomicInteger> topicUsers = new ConcurrentLinkedQueue<AtomicInteger>();
    public Queue<String> message = new ConcurrentLinkedQueue<String>();

    public Topic(String name, AtomicInteger connectionId) {
        this.topicName = name;
        this.topicUsers.add(connectionId);
    }
}
