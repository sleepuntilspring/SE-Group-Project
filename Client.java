import javax.swing.*;
import java.awt.*;
import java.io.*;
import java.net.Socket;

public class Client {
    private JFrame frame;
    private JTextField inputField;
    private JButton sendButton;
    private JLabel resultLabel;

    public Client() {
        frame = new JFrame("Number Incrementer");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(300, 150);
        frame.setLayout(new FlowLayout());

        inputField = new JTextField(10);
        sendButton = new JButton("Increment");
        resultLabel = new JLabel("Result will appear here...");

        sendButton.addActionListener(e -> sendAndReceive(inputField.getText()));

        frame.add(inputField);
        frame.add(sendButton);
        frame.add(resultLabel);

        frame.setVisible(true);
    }

    private void sendAndReceive(String numberText) {
        try (Socket socket = new Socket("localhost", 8080);
             PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            out.println(numberText);
            String response = in.readLine();
            resultLabel.setText("Result: " + response);

        } catch (IOException e) {
            e.printStackTrace();
            resultLabel.setText("Error connecting to server");
        }
    }

    public static void main(String[] args) {
        SwingUtilities.invokeLater(Client::new);
    }
}
