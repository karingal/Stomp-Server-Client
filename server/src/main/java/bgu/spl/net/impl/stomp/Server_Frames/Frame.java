package bgu.spl.net.impl.stomp.Server_Frames;

import java.util.LinkedHashMap;
import java.util.Map;

public class Frame {
    public String body = "";
    public String stompCommand;
    public Map<String,String> headers;

    public Frame() {
        this.body = new String();
        this.stompCommand = new String();
        this.headers = new LinkedHashMap<>();
    }

    public Frame(String[] message) {
        headers = new LinkedHashMap<>();
        stompCommand = message[0];
        int i;
        for (i=1; i<message.length && !message[i].isEmpty(); i++) {
            String line = message[i];
            int index = line.indexOf(":");
            String key = line.substring(0, index);
            String value = line.substring(index+1, line.length());
            headers.put(key.trim(), value.trim());
        }
        if (i<message.length) {
            if (message[i+1].equals("\u0000")) {
                body = "";
            }
            else {
                body += "\n";
                for (int j=i+1; j<message.length; j++) {
                    if (!message[j].equals("\u0000")) {
                        body += message[j];
                        body += "\n";
                    }
                    else {
                        break;
                    }
                }
            }
        }
    }

    public String toString() {
        String ans = stompCommand + "\n";
        for (Map.Entry<String,String> header : headers.entrySet()) {
            ans += header.getKey() + ":" + header.getValue() + "\n";
        }
        ans += body;
        ans += "\u0000";
        return ans;            
    }

    public void addHeader(String header) {
        String[] headerSplited = header.split("\\:");
        String headerName = headerSplited[0];
        String headerValue = headerSplited[1];
        headers.put(headerName, headerValue);
    }

    public void setStompCommand (String command) {
        this.stompCommand = command;
    }
}
