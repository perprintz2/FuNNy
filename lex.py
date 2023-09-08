import re
import matplotlib.pyplot as plt
import numpy as np
import memberFunk as mf
import threading

token_pattern = r"""
(?P<sigmoid>sigmoid)
|(?P<bell>bell)
|(?P<trapez>trapez)
|(?P<triangle>triangle)
|(?P<identifier>[a-zA-Z_][a-zA-Z0-9_]*)
|(?P<ff>[-+]?(\d+(\.\d*)?|\.\d+)([eE][-+]?\d+)?)
|(?P<kollon>:)
|(?P<ckollon>;)
|(?P<open_curly>[(])
|(?P<close_curly>[)])
|(?P<newline>\n)
|(?P<else>.)
"""

token_re = re.compile(token_pattern, re.VERBOSE)

class TokenizerException(Exception):
    pass


def tokenize(text):
    pos = 0
    while True:
        m = token_re.match(text, pos)
        if not m:
            break
        pos = m.end()
        tokname = m.lastgroup
        tokvalue = m.group(tokname)
        yield tokname, tokvalue
    if pos != len(text):
        raise TokenizerException('tokenizer stopped at pos %r of %r' % (
            pos, len(text)))


def find_min_max(data, len):
    minMax = [1.0e10, -1.0e10]
    for val in range(len):
        if minMax[0] > data[val]:
            minMax[0] = data[val]
        if minMax[1] < data[val]:
            minMax[1] = data[val]
    return minMax

def SetGrafData(type, lin, lvar, parm,j):
    if (type == 'sigmoid'):
        MM = find_min_max(parm, 2)
        min = MM[0] - ((MM[1] - MM[0]) / 10.0)
        max = MM[1] + ((MM[1] - MM[0]) / 10.0)
        xaxis = np.arange(min, max, (max - min) / 20.0)
        yaxis = xaxis.copy()
        i = 0
        for x in xaxis:
            yaxis[i] = mf.membersigmoid(parm, x)
            i += 1

    if (type == 'triangle'):
        MM = find_min_max(parm, 3)
        min = MM[0] - ((MM[1] - MM[0]) / 10.0)
        max = MM[1] + ((MM[1] - MM[0]) / 10.0)
        xaxis = np.arange(min, max, (max - min) / 200.0)
        yaxis = xaxis.copy()
        i = 0
        for x in xaxis:
            yaxis[i] = mf.membertri(parm, x)
            i += 1
    if (type == 'bell'):
        MM = find_min_max(parm, 2)
        min = MM[0] - ((MM[1] - MM[0]) / 10.0)
        max = MM[1] + ((MM[1] - MM[0]) / 10.0)
        xaxis = np.arange(min, max, (max - min) / 200.0)
        yaxis = xaxis.copy()
        i = 0
        for x in xaxis:
            yaxis[i] = mf.memberbell(parm, x)
            i += 1
    if (type == 'trapez'):
        MM = find_min_max(parm, 4)
        min = MM[0] - ((MM[1] - MM[0]) / 10.0)
        max = MM[1] + ((MM[1] - MM[0]) / 10.0)
        xaxis = np.arange(min, max, (max - min) / 200.0)
        yaxis = xaxis.copy()
        i = 0
        for x in xaxis:
            yaxis[i] = mf.membertrap(parm, x)
            i += 1




def plotGraph(graphList, j):
    leg= []
    xaxis = [[0 for x in range(20)] for y in range(200)] 
    yaxis = [[0 for x in range(20)] for y in range(200)] 
    
    for k in range(0,j):
        parm= [graphList[k][3],graphList[k][4],graphList[k][5],graphList[k][6],graphList[k][7],graphList[k][8]]
        if (graphList[k][2] == 'sigmoid'):
            MM = find_min_max(parm, 2)
            min = MM[0] - ((MM[1] - MM[0]) / 10.0)
            max = MM[1] + ((MM[1] - MM[0]) / 10.0)
            xaxis[k] = np.arange(min, max, (max - min) / 200.0)
            yaxis[k] = xaxis[k].copy()
            i = 0
            for x in xaxis[k]:
                yaxis[k][i] = mf.membersigmoid(parm, x)
                i += 1

        if (graphList[k][2] == 'triangle'):
            MM = find_min_max(parm, 3)
            min = MM[0] - ((MM[1] - MM[0]) / 10.0)
            max = MM[1] + ((MM[1] - MM[0]) / 10.0)
            xaxis[k] = np.arange(min, max, (max - min) / 200.0)
            yaxis[k] = xaxis[k].copy()
            i = 0
            for x in xaxis[k]:
                yaxis[k][i] = mf.membertri(parm, x)
                i += 1
        if (graphList[k][2] == 'bell'):
            min = parm[0] - parm[1] *4.0
            max = parm[0] + parm[1] *4.0
            xaxis[k] = np.arange(min, max, (max - min) / 200.0)
            yaxis[k] = xaxis[k].copy()
            i = 0
            for x in xaxis[k]:
                yaxis[k][i] = mf.memberbell(parm, x)
                i += 1
        if (graphList[k][2] == 'trapez'):
            MM = find_min_max(parm, 4)
            min = MM[0] - ((MM[1] - MM[0]) / 10.0)
            max = MM[1] + ((MM[1] - MM[0]) / 10.0)
            xaxis[k] = np.arange(min, max, (max - min) / 200.0)
            yaxis[k] = xaxis[k].copy()
            i = 0
            for x in xaxis[k]:
                yaxis[k][i] = mf.membertrap(parm, x)
                i += 1

        #xpoints = np.array(xaxis)
        #ypoints = np.array(yaxis)

        plt.plot(xaxis[k], yaxis[k])
        plt.title("Membership Functions")
        tit= graphList[k][0] + '  ' + graphList[k][1] + ' : ' + graphList[k][2]
        leg.append(tit)
        plt.legend(leg)
    
    plt.show()

  
def inc(i,n,ll):
    if i <= (ll-n):
        i+=n
        return i
    else:
        print('Membership Function plotter\n   Error: Missing data')
        return 0






def plotMember(stuff):
    graphData=["", "", "type", 0.0 , 1.0, 1.0, 1.0, 1.0, 1.0]
    graphList=[]
    i = 0
    j = 0

    list=[]
    for tok in tokenize(stuff):
        if tok[0] != 'else':
            list.append(tok)
        i=0
    ll= len(list)
    while list[i][0] == 'newline':
        i = inc(i,1,ll)
        if not i:
            break

    lrun = True

    while lrun:
        if list[i][0] == 'identifier':
            graphData[0] = list[i][1]
            
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: identifier expected: The read symbol <{}>'.format(list[i][1]))
            break
        if list[i][0] == 'identifier':
            graphData[1] = list[i][1]
            
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: identifier expected: The read symbol <{}>'.format(list[i][1]))
            break
        if list[i][0] == 'kollon':
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: <'':''> expected: The read symbol <{}>'.format(list[i][1]))
            break
        if (list[i][0] == 'sigmoid') or (list[i][0] == 'bell') or (list[i][0] == 'trapez') or (list[i][0] == 'triangle'):
            graphData[2] = list[i][1]
           
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: Membership funk. expected: The read symbol <{}>'.format(list[i][1]))
            break
        if list[i][0] == 'open_curly':
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: ''('' expected: The read symbol <{}>'.format(list[i][1]))
            break
        if (graphData[2] == 'sigmoid') or (graphData[2] == 'bell'):
            n=3
        if (graphData[2] == 'trapez'):
            n=5
        if (graphData[2] == 'triangle'):
            n=4

        for k in range(0,n):
            if list[i][0] == 'ff':
                graphData[k+3] = float(list[i][1])
                i = inc(i,2,ll)
                if not i:
                    break
            else:
                print('Membership Function plotter\n   Error: identifier expected: The read symbol <{}>'.format(list[i][1]))
                break
        if list[i][0] == 'close_curly':
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: '')'' expected: The read symbol <{}>'.format(list[i][1]))
            break
        if list[i][0] == 'ckollon':
            i = inc(i,1,ll)
            if not i:
                break
        else:
            print('Membership Function plotter\n   Error: '';'' expected: The read symbol <{}>'.format(list[i][1]))
            break
        
        if (graphData[0] != ''):
            dd= graphData.copy()
            graphList.append(dd)
            j+=1
        if i < len(list):
            while (list[i][0] == 'newline'):
                i+=1
                if i == len(list):
                    lrun= False
                    break

        else:
            lrun= False
    plotGraph(graphList, j)
