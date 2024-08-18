# CS3205 - Assignment 1

## Team Members: Chahel Singh (CS21B021) and Shaun Mathew (CS21B076)

Google Drive Link of all the pcap and har files used: [Link](https://drive.google.com/drive/folders/1kLLCF1tC3kWIg35g8wRl9GuwQeljtP2j?usp=drive_link)

### Activity 1 - Preliminary analysis of packet capture data

Script used: `pcap_analysis.ipynb`

Methods:
1. set_my_ip():
> Checks the most **frequent** ip in the destination of packets and claims it as host_ip
2. Finding the dns.qry_name:
> Check if an element of the qry_name has the website keyword in it (Eg: **mit** in mit.edu)

Plots are done for:
1. Task A1
    - Kbps vs Time
    - Packets/Second vs Time
    - Fraction of Burst vs Resolution
2. Task A2
    - Packets vs Time

Refer to the notebook for the analysis of the pcap files, plots and neccessary code.

#### Observations on Time to First Byte (TTFB) and Time to Name Resolution (TNF)

> In our analysis, we have observed significant variations in the Time to First Byte (TTFB) and Time to Name Resolution (TNF) across different webpages. Notably, webpages that are less frequently accessed and hosted at distant locations exhibit slower response times compared to others.

### Activity 2 - Preliminary analysis of HTTP Archive (HAR) data

Script used: `har_analysis.ipynb`

Refer to the notebook for the analysis of the har files, plots and neccessary code.

### Activity 3 - DNS packet parser

Script used: `dns.ipynb`

The parser has been created based on the rules in RFC 1035 and the simplifications mentioned in the assignment. The parser allows RCODE to be 0 - 5 and opcode to be 0 - 2. The parser also allows for exactly one question and at most one answer in the DNS packet. In response type packets, it has been assumed that the answer is present. Also, if the answer Domain Name is different from the Question Domain Name, the packet is deemed invalid.

### To use the code
1. Use the drive link mentioned above and keep the .ipynb notebooks in the same folder as the folder pcapture
2. Run the desired notebooks for the outputs