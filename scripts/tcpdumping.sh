sudo tcpdump -w ../tcpdumps/epoll_500.pcap -i lo
sudo tcpdump -w ../tcpdumps/epoll_1000.pcap -i lo
sudo tcpdump -w ../tcpdumps/epoll_3000.pcap -i lo

sudo tcpdump -w ../tcpdumps/poll_500.pcap -i lo
sudo tcpdump -w ../tcpdumps/poll_1000.pcap -i lo
sudo tcpdump -w ../tcpdumps/poll_3000.pcap -i lo

sudo tcpdump -w ../tcpdumps/select_500.pcap -i lo
sudo tcpdump -w ../tcpdumps/select_1000.pcap -i lo
sudo tcpdump -w ../tcpdumps/select_3000.pcap -i lo

sudo tcpdump -w ../tcpdumps/fork_500.pcap -i lo
sudo tcpdump -w ../tcpdumps/fork_1000.pcap -i lo
sudo tcpdump -w ../tcpdumps/fork_3000.pcap -i lo

sudo tcpdump -w ../tcpdumps/pthread_500.pcap -i lo
sudo tcpdump -w ../tcpdumps/pthread_1000.pcap -i lo
sudo tcpdump -w ../tcpdumps/pthread_3000.pcap -i lo

