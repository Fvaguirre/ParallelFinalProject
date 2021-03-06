import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
def parseDataGroupedByRanks():
    f_in = open("results.txt", "r")
    times = dict()
    for line in f_in:
        splitted = line.strip().split(" ")
        if splitted[0] == "#":
            key = " ".join(splitted[1:])
            times[key] = []
            for i in range(0,2):
                line = next(f_in)
                splitted2 = line.strip().split(" ")
                times[key].append(float(splitted2[2]))
    min_sum = 10000000
    ans1 = None
    ans = None
    for key, val in times.items():
        if sum(val) < min_sum:
            min_sum = sum(val)
            ans = key
            ans1 = val
    print("Minimum sum: %f at key: %s with vals: %s " % (min_sum, ans, ans1))
    return times

def generateGraphs(times):
    title = "N nodes vs Encode and Decode Times at "
    serial_times = []
    nodes = []
    encode_time = []
    decode_time = []
    serial_decode = []
    serial_encode = []
    total_mpi = []
    node1 = []
    node2 = []
    node4 = []
    node8 = []
    node16 = []
    node32 = []
    node64 = []
    node128 = []
    all_encode = []
    all_decode = []
    last_ranks = 2
    x_label = "Time"
    y_label = "Nodes"
    for key, val in times.items():
        print(key)
        if key == "1 nodes 1 ranks":
            serial_times = val
            node1.append((val[0], val[1]))
        else:
            splitted_key = key.split(" ")
            print(splitted_key)
            curr_rank = int(splitted_key[2])
            curr_node = int(splitted_key[0])
            if curr_rank != last_ranks:
                # if last_ranks == 2:
                title += "%s MPI Ranks" % (last_ranks)
                # else:
                #     title += "%s MPI Ranks" % (splitted_key[2])
                # print(len(nodes))
                # print(nodes)
                # print(len(encode_time))
                # print(encode_time)
                # print(len(decode_time))
                # print(decode_time)

                # plt.plot(nodes, serial_encode)
                # plt.plot(nodes, serial_decode)
                # plt.plot(nodes, encode_time)
                # plt.plot(nodes, decode_time)
                # plt.title(title)
                # plt.xlabel(y_label)
                # plt.ylabel(x_label)
                # plt.legend(['serial encode time', 'serial decode time', 'encode time', 'decode time'], loc='upper left')
                #
                # plt.grid()
                # plt.show()
                # plt.title("Total MPI Ranks Nodes (1-128) Ranks at %s" %(last_ranks))
                # plt.xlabel("Total MPI Ranks")
                # plt.ylabel("Encode Time")
                # plt.grid()
                # plt.plot(total_mpi, encode_time)
                # # plt.legend(['encode time', 'decode time'], loc='upper right')
                #
                # plt.show()
                #
                # plt.title("Total MPI Ranks Nodes (1-128) Ranks at %s" %(last_ranks))
                # plt.xlabel("Total MPI Ranks")
                # plt.ylabel("Decode Time")
                # plt.grid()
                # plt.plot(total_mpi, decode_time)
                # plt.legend(['encode time', 'decode time'], loc='upper right')

                # plt.show()

                last_ranks = curr_rank
                nodes = []
                encode_time = []
                decode_time = []
                serial_decode = []
                serial_encode = []
                total_mpi = []
                title = "N nodes vs Encode and Decode Times at "

            if splitted_key[0] == '1':
                node1.append((val[0], val[1]))
            elif splitted_key[0] == '2':
                node2.append((val[0], val[1]))
            elif splitted_key[0] == '4':
                node4.append((val[0], val[1]))
            elif splitted_key[0] == '8':
                node8.append((val[0], val[1]))
            elif splitted_key[0] == '16':
                node16.append((val[0], val[1]))
            elif splitted_key[0] == '32':
                node32.append((val[0], val[1]))
            elif splitted_key[0] == '64':
                node64.append((val[0], val[1]))
            elif splitted_key[0] == '128':
                node128.append((val[0], val[1]))
            total_mpi.append(curr_rank*curr_node)
            nodes.append(int(splitted_key[0]))
            encode_time.append(val[0])
            decode_time.append(val[1])
            serial_encode.append(serial_times[0])
            serial_decode.append(serial_times[1])
            all_encode.append(val[0])
            all_decode.append(val[1])




    # plt.xlabel("Total MPI Ranks")
    # plt.ylabel("Encode Time")
    # plt.plot(total_mpi, encode_time)
    #
    #
    # plt.title("Total MPI Ranks Nodes (1-128) Ranks at %s" %(last_ranks))
    #
    # plt.grid()
    # # plt.legend(['encode time', 'decode time'], loc='upper right')
    #
    # plt.show()
    #
    #
    # plt.xlabel("Total MPI Ranks")
    # plt.ylabel("Decode Time")
    # plt.plot(total_mpi, decode_time)
    #
    #
    # plt.title("Total MPI Ranks Nodes (1-128) Ranks at %s" %(last_ranks))
    #
    # plt.grid()
    # plt.show()
    # plt.xlabel(y_label)
    # plt.ylabel(x_label)
    # plt.title("N nodes vs Encode and Decode Times at 256 MPI Ranks")
    # plt.plot(nodes, serial_encode)
    # plt.plot(nodes, serial_decode)
    # plt.plot(nodes, encode_time)
    # plt.plot(nodes, decode_time)
    # plt.legend(['serial encode time', 'serial decode time', 'encode time', 'decode time'], loc='upper left')
    #
    # plt.grid()
    # plt.show()

    print("DONE PRINTING FIXED NODES!!!")
    # ranks = [1, 2, 4, 8, 16, 32, 64, 128, 256]
    # decode = [x[0] for x in node1]
    # encode = [x[1] for x in node1]
    # print(decode)
    # print(len(decode))
    #
    # plt.plot(ranks[:7], encode)
    # plt.plot(ranks[:7], decode)
    # plt.grid()
    # plt.show()
    #
    # decode = [x[0] for x in node2]
    # encode = [x[1] for x in node2]
    # plt.plot(ranks[:len(encode)], encode)
    # plt.plot(ranks[:len(decode)], decode)
    # plt.grid()
    # plt.show()




def plotTotalRanks(times):
    curr_key = None
    last_key = None
    tot_ranks = dict()
    for key, val in times.items():
        splitted = key.split(" ")
        curr_key = int(splitted[0]) * int(splitted[2])
        print(curr_key)
        if curr_key not in tot_ranks:
            tot_ranks[curr_key] = []
            tot_ranks[curr_key].append(val)
        else:
            tot_ranks[curr_key].append(val)
    # print(tot_ranks)
    ranks = []
    encodes = []
    decodes = []
    last_d = 0
    last_e = 0
    serial_e = 0
    serial_d = 0
    for key, val in tot_ranks.items():
        if key == 1:
            serial_e = val[0][0]
            serial_d = val[0][1]
        ranks.append(key)
        avg_encode = 0
        avg_decode = 0
        for v in val:
            avg_encode += v[0]
            avg_decode += v[1]
        avg_encode /= len(val)
        avg_decode /= len(val)
        encodes.append(avg_encode)
        print("Average encode for %d total MPI ranks: %f" %(key, avg_encode))
        decodes.append(avg_decode)
        print("Average decode for %d total MPI ranks: %f" %(key, avg_decode))
        print("Speedup in encode is %f" %(float(last_e/avg_encode)))
        print("Speedup in decode is %f" %(float(last_d/avg_decode)))
        print("Encode speedup compared to sequential: %f" % (serial_e/avg_encode))
        print("Decode speedup compared to sequential: %f" % (serial_d/avg_decode))

        last_d = avg_decode
        last_e = avg_encode
        print()

    df = pd.DataFrame(np.c_[encodes, decodes], index=ranks)
    ax = df.plot.bar()
    ax.set_xlabel("Total MPI Ranks")
    ax.set_ylabel("Time")
    ax.set_title("Total MPI Ranks vs Average Running Time")
    ax.legend(["Encode Time", "Decode Time"])

    plt.show()
    print(len(ranks))
    print(len(encodes))
    print(len(decodes))




def main():
    times = parseDataGroupedByRanks()
    # generateGraphs(times)
    plotTotalRanks(times)


if __name__ == '__main__':
    main()
