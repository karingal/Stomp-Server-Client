package bgu.spl.net.impl.stomp;

import java.util.concurrent.atomic.AtomicInteger;

import bgu.spl.net.impl.stomp.Server_Frames.Frame;
import bgu.spl.net.srv.ConnectionsImpl;

public class StompMessagingProtocolImpl implements StompMessagingProtocol<Frame> {
    private boolean shouldTerminate = false;
    public AtomicInteger connectionId;
    public ConnectionsImpl<Frame> connections;
    public static Users users = new Users();
    public int messageCounter = 0;

    public void start(AtomicInteger connectionId, ConnectionsImpl<Frame> connections) {
        this.connectionId = connectionId;
        this.connections = connections; 
    }

    @Override
    public Frame process(Frame message) {
        String command = message.stompCommand;

        if (command.equals("DISCONNECT")) {
            Frame response = createReceiptFrame(message);
            User currUser = users.ConnectionIdToUser.get(connectionId);
            String userName = currUser.login;
            users.connectedUsers.remove(userName);
            connections.send(connectionId,response);
            connections.disconnect(connectionId);
            shouldTerminate = true;
        }

        else if (command.equals("CONNECT")) {
            Frame response = loginFrame(message);
            connections.send(connectionId,response);
            if (response.stompCommand.equals("ERROR")) {
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
        }

        else if (command.equals("SEND")) {
            String topicName = message.headers.get("destination");

            if (connections.topicsNames.contains(topicName)) {
                if (connections.topicsToTopicsName.get(topicName) != null) {
                    Topic topic = connections.topicsToTopicsName.get(topicName);
                    if (topic.topicUsers.contains(connectionId)) {
                        for (AtomicInteger currConnecetionId : topic.topicUsers) {
                            if (users.ConnectionIdToUser.get(connectionId) != null) {
                                User currUser = users.ConnectionIdToUser.get(connectionId);
                                if (currUser.TopicToSubId.get(topicName) != null) {
                                    String subId = currUser.TopicToSubId.get(topicName);
                                    Frame response = createMessageFrame(message, subId);
                                    connections.send(currConnecetionId, response);
                                }
                            }
                        }
                    }    
                    else if (!topic.topicUsers.contains(connectionId)) {
                        Frame error = createErrorFrameToSend("Client not subscribed", message);
                        connections.send(connectionId, error);
                        connections.disconnect(connectionId);
                        shouldTerminate = true;
                    }
                }
            }
            else {
                Frame error = createErrorFrameToSend("Topic doesn't exists", message);
                connections.send(connectionId,error);
                connections.disconnect(connectionId);
                shouldTerminate = true;
            }
        }

        else if (command.equals("SUBSCRIBE")) {
            String topicName = message.headers.get("destination");
            String subId = message.headers.get("id");

            if (connections.topicsNames.contains(topicName)) {
                if (connections.topicsToTopicsName.get(topicName) != null) {
                    Topic topic = connections.topicsToTopicsName.get(topicName);
                    topic.topicUsers.add(connectionId);
                    if (users.ConnectionIdToUser.get(connectionId) != null) {
                        User currUser = users.ConnectionIdToUser.get(connectionId);
                        currUser.TopicToSubId.put(topicName, subId);
                        currUser.SubIdToTopic.put(subId, topicName);
                        currUser.subIdList.add(subId);
                    }
                }
            }

            else {
                connections.topicsNames.add(topicName);
                Topic newTopic = new Topic(topicName,connectionId);
                connections.topicsToTopicsName.put(topicName, newTopic);
                if (users.ConnectionIdToUser.get(connectionId) != null) {
                    User currUser = users.ConnectionIdToUser.get(connectionId);
                    currUser.TopicToSubId.put(topicName, subId);
                    currUser.SubIdToTopic.put(subId, topicName);
                    currUser.subIdList.add(subId);
                }
            }
        }

        else if(command.equals("UNSUBSCRIBE")) {
            String subId = message.headers.get("id");

            if (users.ConnectionIdToUser.get(connectionId) != null) {
                User currUser = users.ConnectionIdToUser.get(connectionId);
                if (currUser.SubIdToTopic.get(subId) != null) {
                    String topicName = currUser.SubIdToTopic.get(subId);
                    Topic topic = connections.topicsToTopicsName.get(topicName);
                    topic.topicUsers.remove(connectionId);
                }
                else {
                    Frame error = createErrorFrameToUnsubscribe(message);
                    connections.send(connectionId, error);
                    connections.disconnect(connectionId);
                    shouldTerminate = true;
                }
            }
        }
        return null;
    }


    private Frame createErrorFrameToUnsubscribe(Frame message) {
        Frame errorFrame = new Frame();
        errorFrame.stompCommand = "ERROR";

        errorFrame.addHeader("message: Subscribtion id doesn't exist");

        errorFrame.body = "";
        return errorFrame;
    }

    private Frame createErrorFrameToSend(String string, Frame message) {
        Frame errorFrame = new Frame();
        errorFrame.stompCommand = "ERROR";

        if (string.equals("Client not subscribed")){
            errorFrame.addHeader("message: Client is not subscribed to this topic");
        }
        else if (string.equals("Topic doesn't exists")) {
            errorFrame.addHeader("message: Topic doesn't exist");
        }

        errorFrame.body = "";
        return errorFrame;
    }

    private Frame createMessageFrame(Frame message, String subId) {
        Frame messageFrame = new Frame();
        String destination = message.headers.get("destination");
        messageFrame.stompCommand = "MESSAGE";
        messageFrame.addHeader("destination:" + destination);
        messageFrame.addHeader("subscription:" + subId);
        messageFrame.addHeader("message-id:" + messageCounter);
        messageFrame.body = message.body;
        messageCounter++;
        return messageFrame;
    }

    private Frame createReceiptFrame(Frame message) {
        Frame receiptFrame = new Frame();
        String receiptId = message.headers.get("receipt");
        receiptFrame.setStompCommand("RECEIPT");
        receiptFrame.addHeader("receipt-id:" + receiptId);
        return receiptFrame;
    }

    private Frame loginFrame(Frame message) { 
        String passcode = message.headers.get("passcode");
        String login = message.headers.get("login");

        if (users.connectedUsers.contains(login)) {
            return createErrorFrameToConnect("User already logged in", message);
        }

        else if (users.allUsers.get(login) != null && !users.allUsers.get(login).equals(passcode)) {
            return createErrorFrameToConnect("Wrong password" ,message);
        }

        else if (users.allUsers.get(login) == null) {
            User user = new User(connectionId,login,passcode);
            users.allUsers.put(login,passcode);
            users.connectedUsers.add(login);
            users.ConnectionIdToUser.put(connectionId, user);
            Frame ans = createConnectedFrame();
            return ans;
        }

        else if (users.allUsers.get(login) != null && users.allUsers.get(login).equals(passcode)) {
            User user = new User(connectionId,login,passcode);
            users.ConnectionIdToUser.put(connectionId, user);
            users.connectedUsers.add(login);
            Frame ans = createConnectedFrame();
            return ans;
        }
        else {
            return null;
        }

    }

    private Frame createConnectedFrame() {
        Frame connectedFrame = new Frame();
        connectedFrame.stompCommand = "CONNECTED";
        connectedFrame.addHeader("version:1.2");
        return connectedFrame;
    }

    private Frame createErrorFrameToConnect(String string, Frame message) {
        Frame errorFrame = new Frame();
        errorFrame.stompCommand = "ERROR";
        errorFrame.body = "";
        if (string.equals("Wrong passcode")){
            errorFrame.addHeader("message: wrong passcode");
        }
        else if (string.equals("User already logged in")) {
            errorFrame.addHeader("message: user already logged in");
        }
        return errorFrame;
    }

    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

}
