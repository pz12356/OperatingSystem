import java.util.LinkedList;
import java.util.Queue;

public class Main {
    public static void main(String[] args) {
        Monitor monitor = new Monitor(10);
        Thread producer = new Thread(new Producer(monitor));
        Thread consumer = new Thread(new Consumer(monitor));
        producer.start();
        consumer.start();
    }
    static class Monitor {
        private final int n;
        private Queue<Integer> q;
        public Monitor(int n) {
            this.n = n;
            q = new LinkedList<>();
        }
        public synchronized void insert(int item) {
            while (q.size() == n) {
                try {
                    this.wait();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
            System.out.println("Producer add " + item + " to Buffer");
            q.offer(item);
            this.notifyAll();
        }
        public synchronized Integer remove() {
            while (q.isEmpty()) {
                try {
                    this.wait();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
            Integer item = q.poll();
            System.out.println("\t\t\t\t Consumer remove " + item + " from Buffer");
            this.notifyAll();
            return item;
        }

        public int getN() {
            return n;
        }
    }
    static class Producer implements Runnable {
        private final Monitor monitor;
        public Producer(Monitor monitor) {
            this.monitor = monitor;
        }
        @Override
        public void run() {
            int ItemNumber = monitor.getN() * 3;
            for (int i = 0; i < ItemNumber; i++) {
                int item = i + 1;
                monitor.insert(item);
            }
        }
    }
    static class Consumer implements Runnable {
        private final Monitor monitor;
        public Consumer(Monitor monitor) {
            this.monitor = monitor;
        }

        @Override
        public void run() {
            int ItemNumber = monitor.getN() * 3;
            for (int i = 0; i < ItemNumber; i++) {
                monitor.remove();
            }
        }
    }
}