L = [
[1, 1],
[0.6830127, 1.1830127, 0.3169873, -0.1830127],
[0.47046721, 1.14111692, 0.650365, -0.19093442, -0.12083221, 0.0498175],
[0.32580343, 1.01094572, 0.8922014, -0.03957503, -0.26450717, 0.0436163, 0.0465036, -0.01498699],
[0.22641898, 0.85394354, 1.02432694, 0.19576696, -0.34265671, -0.04560113, 0.10970265, -0.00882680, -0.01779187, 4.71742793e-3],
[0.15774243, 0.69950381, 1.06226376, 0.44583132, -0.31998660, -0.18351806, 0.13788809, 0.03892321, -0.04466375, 7.83251152e-4, 6.75606236e-3, -1.52353381e-3],
[0.11009943, 0.56079128, 1.03114849, 0.66437248, -0.20351382, -0.31683501, 0.1008467, 0.11400345, -0.05378245, -0.02343994, 0.01774979, 6.07514995e-4, -2.54790472e-3, 5.00226853e-4],
[0.07695562, 0.44246725, 0.95548615, 0.82781653, -0.02238574, -0.40165863, 6.68194092e-4, 0.18207636, -0.02456390, -0.06235021, 0.01977216, 0.01236884, -6.88771926e-3, -5.54004549e-4, 9.55229711e-4, -1.66137261e-4],
[0.05385035, 0.34483430, 0.85534906, 0.92954571, 0.18836955, -0.41475176, -0.13695355, 0.21006834, 0.043452675, -0.09564726, 3.54892813e-4, 0.03162417, -6.67962023e-3, -6.05496058e-3, 2.61296728e-3, 3.25814671e-4, -3.56329759e-4, 5.5645514e-5],
[0.03771716, 0.26612218, 0.74557507, 0.97362811, 0.39763774, -0.35333620, -0.27710988, 0.18012745, 0.13160299, -0.10096657, -0.04165925, 0.04696981, 5.10043697e-3, -0.01517900, 1.97332536e-3, 2.81768659e-3, -9.69947840e-4, -1.64709006e-4, 1.32354367e-4, -1.875841e-5]
]

for CL in L:
    CH = [(-1)**k * CL[len(CL) - k - 1] for k in xrange(len(CL))]
    
    iCL = []
    iCH = []
    
    for k in xrange(len(CL) - 1, 0, -2):
        #print(k - 2)
        #print(k - 1)
        iCL = iCL + [CL[k-1], CH[k-1]]
        iCH = iCH + [CL[k], CH[k]]
    
    print('\ndouble CL' + str(len(CL)) + '[' + str(len(CL)) + '] = {\n')
    for i in CL[:-1]:
        print('    ' + '{:<16}'.format(str(i if i > 0 else i * -1)) + '/' + (' ' if i > 0 else '-') + 'sqrt(2) ,\n')
    print('    ' + '{:<16}'.format(str(CL[-1] if CL[-1] > 0 else CL[-1] * -1)) + '/' + (' ' if CL[-1] > 0 else '-') + 'sqrt(2) };\n')
    
    print('double CH' + str(len(CL)) + '[' + str(len(CL)) + '] = {\n')
    for i in CH[:-1]:
        print('    ' + '{:<16}'.format(str(i if i > 0 else i * -1)) + '/' + (' ' if i > 0 else '-') + 'sqrt(2) ,\n')
    print('    ' + '{:<16}'.format(str(CH[-1] if CH[-1] > 0 else CH[-1] * -1)) + '/' + (' ' if CH[-1] > 0 else '-') + 'sqrt(2) };\n')
    
    print('double iCL' + str(len(CL)) + '[' + str(len(CL)) + '] = {\n')
    for i in iCL[:-1]:
        print('    ' + '{:<16}'.format(str(i if i > 0 else i * -1)) + '/' + (' ' if i > 0 else '-') + 'sqrt(2) ,\n')
    print('    ' + '{:<16}'.format(str(iCL[-1] if iCL[-1] > 0 else iCL[-1] * -1)) + '/' + (' ' if iCL[-1] > 0 else '-') + 'sqrt(2) };\n')
    
    print('double iCH' + str(len(CL)) + '[' + str(len(CL)) + '] = {\n')
    for i in iCH[:-1]:
        print('    ' + '{:<16}'.format(str(i if i > 0 else i * -1)) + '/' + (' ' if i > 0 else '-') + 'sqrt(2) ,\n')
    print('    ' + '{:<16}'.format(str(iCH[-1] if iCH[-1] > 0 else iCH[-1] * -1)) + '/' + (' ' if iCH[-1] > 0 else '-') + 'sqrt(2) };\n')