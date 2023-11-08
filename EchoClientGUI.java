import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.Socket;

public class EchoClientGUI {
    private JFrame frame;
    private JTextField inputField;
    private JTextArea outputArea;
    private JButton sendButton;

    public EchoClientGUI() {
        frame = new JFrame("Echo Input");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(400, 200);
        frame.setLayout(new FlowLayout());

        inputField = new JTextField(20);
        outputArea = new JTextArea(5, 20);
        outputArea.setEditable(false);

        sendButton = new JButton("Send");
        sendButton.addActionListener(e -> sendAndReceive(inputField.getText()));

        frame.add(inputField);
        frame.add(sendButton);
        frame.add(new JScrollPane(outputArea));

        frame.setVisible(true);
    }

    private void sendAndReceive(String text) {
        try (Socket socket = new Socket("localhost", 8080);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {
            
            out.println(text);
            String response = in.readLine();
            outputArea.setText(response);

        } catch (IOException e) {
            e.printStackTrace();
            outputArea.setText("Error connecting to server");
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(EchoClientGUI::new);
    }
}
