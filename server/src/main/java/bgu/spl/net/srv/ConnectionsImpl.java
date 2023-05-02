package bgu.spl.net.srv;

import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.impl.stomp.Topic;

public class ConnectionsImpl<T> implements Connections<T> {

    public List<ConnectionHandler<T>> synchronizedChList = Collections.synchronizedList(new LinkedList<ConnectionHandler<T>>());
    public LinkedList<String> topicsNames = new LinkedList<String>();
    public Map<String,Topic> topicsToTopicsName = new HashMap<>(); 

    public boolean send(AtomicInteger connectionId, T msg) {
        ConnectionHandler<T> ch = synchronizedChList.get(connectionId.get());
        if (ch != null) {
            ch.send(msg);
        }
        return true;
    }

    public void send(String channel, T msg) {
        for (String topic : topicsNames) {
            if (topic.equals(channel)) {
                Topic currTopic = topicsToTopicsName.get(topic);
                Queue<AtomicInteger> topicUsers = currTopic.topicUsers;
                for (AtomicInteger connectionId : topicUsers) {
                    send(connectionId,msg);
                }
            }
        }
    }

    public void disconnect(AtomicInteger connectionId) {
        synchronizedChList.remove(connectionId.get());
        synchronizedChList.add(connectionId.get(), null);
        for (String topic : topicsNames) {
            Topic currTopic = topicsToTopicsName.get(topic);
            Queue<AtomicInteger> topicUsers = currTopic.topicUsers;
            if (topicUsers.contains(connectionId)) {
                topicUsers.remove(connectionId);
            }
        }
    }

}