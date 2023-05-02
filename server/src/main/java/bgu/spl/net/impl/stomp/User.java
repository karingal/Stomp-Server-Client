package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;


public class User {
    public AtomicInteger connectionId;
    public String login;
    public String passcode;
    public LinkedList<String> subIdList = new LinkedList<String>();
    public Map<String,String> TopicToSubId = new HashMap<>();
    public Map<String,String> SubIdToTopic = new HashMap<>();

    public User(AtomicInteger counter, String userName, String passcode) {
        this.connectionId = counter;
        this.login = userName;
        this.passcode = passcode;
    }

}