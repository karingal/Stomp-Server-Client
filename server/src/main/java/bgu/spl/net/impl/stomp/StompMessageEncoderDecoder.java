package bgu.spl.net.impl.stomp;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.impl.stomp.Server_Frames.Frame;


public class StompMessageEncoderDecoder<T> implements MessageEncoderDecoder<Frame> {

    private byte[] bytes = new byte[1 << 10];
    private int len = 0;

    @Override
    public Frame decodeNextByte(byte nextByte) {
        if (nextByte == '\u0000') {
            pushByte(nextByte);
            String message = popString();
            String[] splittedRow = message.split("\n");
            Frame newFrame = new Frame(splittedRow);
            return newFrame;
        }
        pushByte(nextByte);
        return null; //not a line yet
    }

    @Override
    public byte[] encode(Frame message) {
        return message.toString().getBytes();
    }

    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }

        bytes[len++] = nextByte;
    }

    private String popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        return result;
    }

}

