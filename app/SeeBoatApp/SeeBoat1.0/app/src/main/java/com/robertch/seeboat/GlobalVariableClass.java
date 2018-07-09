package com.robertch.seeboat;


/*
This class enables getters and setters for having persistent data across threads.

ie, If you want to make a value global, that can synchronize, getters and setters allow a way of doing this.
 */

class GlobalVariableClass {
    private static GlobalVariableClass mInstance = null;

    private boolean killThreads;

    private GlobalVariableClass() {
    }

    static synchronized GlobalVariableClass getInstance() {
        if (null == mInstance) {
            mInstance = new GlobalVariableClass();
        }
        return mInstance;
    }

    boolean getKillThreads() {
        return killThreads;
    }

    void setKillThreads(boolean state) {
        killThreads = state;
    }

}
