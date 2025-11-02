public class MainActivity extends AppCompatActivity {
    private static final String SERVER_IP = "192.168.1.100"; // ваш IP
    private static final int SERVER_PORT = 12345;
    
    private EditText messageInput;
    private TextView responseText;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        messageInput = findViewById(R.id.message_input);
        responseText = findViewById(R.id.response_text);
        
        findViewById(R.id.send_button).setOnClickListener(v -> sendMessage());
    }
    
    private void sendMessage() {
        new Thread(() -> {
            try {
                Socket socket = new Socket(SERVER_IP, SERVER_PORT);
                
                // Отправка сообщения
                String message = messageInput.getText().toString();
                OutputStream out = socket.getOutputStream();
                out.write(message.getBytes());
                out.flush();
                
                // Получение ответа
                InputStream in = socket.getInputStream();
                byte[] buffer = new byte[1024];
                int bytesRead = in.read(buffer);
                String response = new String(buffer, 0, bytesRead);
                
                // Показываем ответ в UI потоке
                runOnUiThread(() -> responseText.setText("Server: " + response));
                
                socket.close();
            } catch (IOException e) {
                e.printStackTrace();
                runOnUiThread(() -> responseText.setText("Error: " + e.getMessage()));
            }
        }).start();
    }
}
