package com.xtase;

/**
 * Created by fgalliat on 14/05/18.
 */
public class SystemPrinter implements IPrinter {
    @Override
    public void print(Object o) {
        System.out.print(o);
    }

    @Override
    public void println(Object o) {
        System.out.println(o);
    }
}
