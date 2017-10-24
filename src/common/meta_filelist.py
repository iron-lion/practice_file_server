#!/usr/bin/python
FILE_ADDRESS = "META_FILE.list"
def make_filedic():
    user_dic = {}
    try:
        FD = open(FILE_ADDRESS, "r")
    except:
        return user_dic

    for line in FD.readlines():
        block = line.rstrip("\n").split("\t")
        try:
            tmp_set = user_dic[block[0]]
        except KeyError:
            tmp_set = set()
        tmp_set.add((block[1],block[2],))
        user_dic[block[0]] = tmp_set
    FD.close()
    print user_dic
    return user_dic


def exist(user_name, filename):
    user_file_dic = make_filedic()
#    print "[P] exist:", filename
    try:
        tmp_set = user_file_dic[user_name]
        for i in tmp_set:
#            print "[P]: ",i[0]
            if filename == i[0]:
                return str(i[1])
    except KeyError:
        return 0
    return 0


def write(user_name, filename, pieces):
    if not exist(user_name, filename):
        try:
            FD = open(FILE_ADDRESS, "a")
        except:
            FD = open(FILE_ADDRESS, "w")
        print >> FD, user_name+"\t"+filename+"\t"+pieces
        FD.close()
    return 1

def list(user_name):
    tmp_stream = "=====================List==================="
    user_file_dic = make_filedic()
    try:
        tmp_set = user_file_dic[str(user_name)]
        for i in tmp_set:
            tmp_stream += "\n*  " + i[0]
    except KeyError:
        tmp_stream += "\nEMPTY"
    tmp_stream += "\n=====================End===================="
    return tmp_stream
