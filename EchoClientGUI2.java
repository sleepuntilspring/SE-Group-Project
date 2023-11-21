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
        frame.setSize(600, 300);
        frame.setLayout(new GridLayout(1, 2));

        JPanel leftPanel = new JPanel();
        leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));

        JMenuBar leftMenuBar = new JMenuBar();
        JMenu m1 = new JMenu("File");
        JMenu m2 = new JMenu("Help");
        JMenu m3 = new JMenu("Edit");
        JMenu m4 = new JMenu("Window");
        leftMenuBar.add(m1);
        leftMenuBar.add(m2);
        leftMenuBar.add(m3);
        leftMenuBar.add(m4);
        JMenuItem m11 = new JMenuItem("Open");
        JMenuItem m22 = new JMenuItem("Save");
        JMenuItem m33 = new JMenuItem("Save as");
        JMenuItem m44 = new JMenuItem("New Text File");
        JMenuItem m55 = new JMenuItem("Report Issue");
        JMenuItem m66 = new JMenuItem("Find in Files");
        JMenuItem m77 = new JMenuItem("Replace in Files");
        JMenuItem m88 = new JMenuItem("Minimize");
        JMenuItem m99 = new JMenuItem("Zoom");
        m1.add(m11);
        m1.add(m22);
        m1.add(m33);
        m1.add(m44);
        m2.add(m55);
        m3.add(m66);
        m3.add(m77);
        m4.add(m88);
        m4.add(m99);
        leftPanel.add(leftMenuBar);

        inputField = new JTextField(20);
        sendButton = new JButton("Run");
        sendButton.addActionListener(e -> sendAndReceive(inputField.getText()));
        leftPanel.add(inputField);
        leftPanel.add(sendButton);

        leftPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        frame.add(leftPanel);

        JPanel rightPanel = new JPanel(new BorderLayout());
        outputArea = new JTextArea(10, 30);
        outputArea.setEditable(false);
        rightPanel.add(new JScrollPane(outputArea), BorderLayout.CENTER);

        rightPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));
        frame.add(rightPanel);

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