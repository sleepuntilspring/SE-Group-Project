import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.io.*;
import java.net.Socket;
import java.nio.file.Files;

public class AssemblyTranslatorGUI2 {
    private JFrame frame;
    private JTextArea inputArea;  // For assembly script
    private JTextArea outputArea; // For translation result
    private JButton translateButton;
    private JMenuBar menuBar;
    private JMenu fileMenu;
    private JMenuItem openItem;

    public AssemblyTranslatorGUI2() {
        frame = new JFrame("Assembly Translator");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(800, 400);
        frame.setLayout(new GridLayout(1, 2)); // Splitting the frame into two columns

        inputArea = new JTextArea(20, 40);
        outputArea = new JTextArea(20, 40);
        outputArea.setEditable(false);

        translateButton = new JButton("Translate");
        translateButton.addActionListener(e -> translateAssemblyCode());

        menuBar = new JMenuBar();
        fileMenu = new JMenu("File");
        openItem = new JMenuItem("Open");
       

        openItem.addActionListener(this::openFile);
      

        fileMenu.add(openItem);
        menuBar.add(fileMenu);
        frame.setJMenuBar(menuBar);

        JPanel leftPanel = new JPanel(new BorderLayout());
        leftPanel.add(new JScrollPane(inputArea), BorderLayout.CENTER);
        leftPanel.add(translateButton, BorderLayout.SOUTH);

        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.add(new JScrollPane(outputArea), BorderLayout.CENTER);

        frame.add(leftPanel);
        frame.add(rightPanel);

        frame.setVisible(true);
    }

    private void openFile(ActionEvent e) {
        JFileChooser fileChooser = new JFileChooser();
        int result = fileChooser.showOpenDialog(frame);
        if (result == JFileChooser.APPROVE_OPTION) {
            File selectedFile = fileChooser.getSelectedFile();
            try {
                inputArea.setText(new String(Files.readAllBytes(selectedFile.toPath())));
            } catch (IOException ioException) {
                ioException.printStackTrace();
            }
        }
    }

   

    private void translateAssemblyCode() {
        try (Socket socket = new Socket("localhost", 8080);
             BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {
        
            // Receive the file content from the C program
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = in.readLine()) != null) {
                response.append(line).append("\n");
            }
    
            // Display the result in the output area
            outputArea.setText(response.toString());
    
        } catch (IOException e) {
            e.printStackTrace();
            outputArea.setText("Error connecting to server");
        }
    }
    
    
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(AssemblyTranslatorGUI2::new);
    }
}
