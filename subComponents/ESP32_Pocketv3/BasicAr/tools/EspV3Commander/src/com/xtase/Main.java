package com.xtase;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;

import static java.awt.SystemColor.text;

public class Main implements IPrinter {

    public static void main(String[] args) throws Exception {
        String ip = "192.168.4.1";

        // ===== TEMP =====
        // ip = "127.0.0.1";
        // ip = "192.168.1.11";
        // ===== TEMP =====

        if (args != null && args.length > 0) {
            ip = args[0];
        }

        new Main().doWork(ip);
    }

    protected JFrame win = null;
    protected JTextArea console = null;
    protected JTextArea consoleIn = null;

    protected EspPckV3Telnet mcu = null;

    public void doWork(final String mcuIP) throws Exception {
        JButton uploadBtn = new JButton("UPLOAD");
        uploadBtn.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if (mcu == null) {
                    println("NO MCU connected");
                } else {
                    final JFileChooser choice = new JFileChooser();
                    choice.setCurrentDirectory(new File("."));
                    int ret = choice.showOpenDialog(win);
                    // println("ret=" + ret);

                    if (ret == JFileChooser.APPROVE_OPTION) {
                        new Thread() {
                            public void run() {
                                println("Ya selected " + choice.getSelectedFile().getName());
                                try {
                                    mcu.sendSomething(choice.getSelectedFile().getAbsolutePath(), false);
                                } catch (Exception ex) {
                                    println(ex.toString());
                                    ex.printStackTrace();
                                }
                            }
                        }.start();
                    }
                }
            }
        });

        console = new JTextArea(25, 80);
        consoleIn = new JTextArea(3, 80);

        uploadBtn.setBackground(Color.darkGray);
        uploadBtn.setForeground(Color.cyan.darker());

        console.setBackground(Color.darkGray);
        console.setForeground(Color.cyan.darker());

        consoleIn.setBackground(Color.darkGray);
        consoleIn.setForeground(Color.cyan.darker());


        consoleIn.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent keyEvent) {

            }

            @Override
            public void keyPressed(KeyEvent keyEvent) {
                if (keyEvent.getKeyChar() == '\n') {
                    String content = consoleIn.getText();
                    println(content);
                    try {
                        mcu.println(content);
                    } catch (Exception ex) {
                        println("mcu not linked");
                    }
                    consoleIn.setText("");
                }
            }

            @Override
            public void keyReleased(KeyEvent keyEvent) {

            }
        });


        win = new JFrame("Xts-uBASIC Pocket v3 [" + mcuIP + "]");
        win.getContentPane().setLayout(new BorderLayout());
        win.getContentPane().add(new JScrollPane(console), BorderLayout.CENTER);
        win.getContentPane().add(new JScrollPane(consoleIn), BorderLayout.SOUTH);
        win.getContentPane().add(uploadBtn, BorderLayout.NORTH);

        win.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosing(WindowEvent windowEvent) {
                super.windowClosing(windowEvent);
                halt();
            }
        });

        win.pack();
        win.setVisible(true);

        new Thread() {
            @Override
            public void run() {
                try {
                    mcu = new EspPckV3Telnet(mcuIP);
                    print("MCU connected" + "\n");

                    String line;
                    while ((line = mcu.readLine()) != null) {
                        println(line);
                    }


                } catch (Exception ex) {
                    print("MCU NOT connected !" + "\n");
                    print(ex.toString() + "\n");
                }
            }
        }.start();

    }

    protected void halt() {
        try {
            mcu.close();
        } catch (Exception ex) {
        }
        System.exit(0);
    }

    @Override
    public void print(Object o) {
        console.append("" + o);
    }

    @Override
    public void println(Object o) {
        console.append(o + "\n");
    }
}
