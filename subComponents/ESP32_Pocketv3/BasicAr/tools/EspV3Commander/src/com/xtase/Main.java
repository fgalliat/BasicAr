package com.xtase;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.File;
import java.util.*;


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

            protected File curDir = null;

            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                if (mcu == null) {
                    println("NO MCU connected");
                } else {
                    final JFileChooser choice = new JFileChooser();

                    if (curDir == null) {
                        curDir = new File(".");
                    }

                    choice.setCurrentDirectory(curDir);
                    int ret = choice.showOpenDialog(win);
                    // println("ret=" + ret);

                    if (ret == JFileChooser.APPROVE_OPTION) {
                        new Thread() {
                            public void run() {
                                println("Ya selected " + choice.getSelectedFile().getName());

                                curDir = choice.getSelectedFile().getParentFile();

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
                consoleIn.requestFocus();
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

        console.addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent keyEvent) {

            }

            @Override
            public void keyPressed(KeyEvent keyEvent) {
                consoleIn.requestFocus();
                consoleIn.append("" + keyEvent.getKeyChar());
            }

            @Override
            public void keyReleased(KeyEvent keyEvent) {

            }
        });


        consoleIn.addKeyListener(new KeyListener() {

            protected java.util.List<String> history = new LinkedList<String>();
            protected int hCursor = 1;

            @Override
            public void keyTyped(KeyEvent keyEvent) {

            }

            @Override
            public void keyPressed(KeyEvent keyEvent) {
                if (keyEvent.getKeyCode() == KeyEvent.VK_UP) {
                    try {
                        consoleIn.setText(history.get(history.size() - hCursor));
                    } catch (Exception ex) {
                    }
                    hCursor++;
                } else if (keyEvent.getKeyCode() == KeyEvent.VK_DOWN) {
                    try {
                        consoleIn.setText(history.get(history.size() + hCursor));
                    } catch (Exception ex) {
                    }
                    hCursor--;
                } else if (keyEvent.getKeyChar() == '\n') {
                    String content = consoleIn.getText().trim();
                    println(content);
                    history.add( content );
                    try {
                        mcu.println(content);
                    } catch (Exception ex) {
                        println("mcu not linked");
                    }
                    consoleIn.setText("");
                    hCursor = 1;
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

        consoleIn.requestFocus();

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
