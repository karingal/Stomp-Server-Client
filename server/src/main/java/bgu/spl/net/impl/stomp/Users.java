package bgu.spl.net.impl.stomp;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;


public class Users {

    public Map<Integer,String> SubIdToTopic = new HashMap<>();

    //user Names
    public List<String> connectedUsers = new LinkedList<String>();

    //userName To Password
    public Map<String,String> allUsers = new HashMap<>(); 

    //connecetionId to user 
    public Map<AtomicInteger,User> ConnectionIdToUser = new HashMap<>();


}