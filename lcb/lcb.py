
from parser import stpcommands
from ciphers.cipher import AbstractCipher


class LCBCipher(AbstractCipher):
    name = "lcb"

    def getFormatString(self):
        """
        Returns the print format.
        """
        # return ['S', 'P', 'w']
        return ['S', 'P', 'w']

    def createSTP(self, stp_filename, parameters):

        wordsize = parameters["wordsize"]
        rounds = parameters["rounds"]
        weight = parameters["sweight"]

        with open(stp_filename, 'w') as stp_file:
            header = ("% Input File for STP\n% LCB w={}"
                      "rounds={}\n\n\n".format(wordsize, rounds))
            stp_file.write(header)

            # Setup variables
            # s = sbox (non-linear), p = pbox+lbox+swap (linear)
            s = ["S{}".format(i) for i in range(rounds + 1)]
            p = ["P{}".format(i) for i in range(rounds + 1)]

            # w = weight
            w = ["w{}".format(i) for i in range(rounds)]

            # stpcommands.setupVariables(stp_file, s, wordsize)
            # stpcommands.setupVariables(stp_file, p, wordsize)
            stpcommands.setupVariables(stp_file, s, wordsize)
            stpcommands.setupVariables(stp_file, p, wordsize)

            stpcommands.setupVariables(stp_file, w, wordsize)
            stpcommands.setupWeightComputation(stp_file, weight, w, wordsize)

            for i in range(rounds):
                self.setupLcbRound(stp_file, s[i], p[i], s[i+1],
                                      w[i])

            # No all zero characteristic
            stpcommands.assertNonZero(stp_file, s, wordsize)

            # Iterative characteristics only
            # Input difference = Output difference
            if parameters["iterative"]:
                stpcommands.assertVariableValue(stp_file, s[0], s[rounds])

            for key, value in parameters["fixedVariables"].items():
                stpcommands.assertVariableValue(stp_file, key, value)

            for char in parameters["blockedCharacteristics"]:
                stpcommands.blockCharacteristic(stp_file, char, wordsize)

            stpcommands.setupQuery(stp_file)

        return

    def setupLcbRound(self, stp_file, s_in, p, s_out, w):

        command = ""

        # Substitution Layer
        lcb_sbox = [0x0, 0x4, 0x1, 0x5, 0x2, 0x6, 0x3, 0x7,
                       0x8, 0xc, 0x9, 0xd, 0xa, 0xe, 0xb, 0xf]

        twine_sbox = [0xC, 0, 0xF, 0xA, 2, 0xB, 9, 5, 8, 3, 0xD, 7, 1, 0xE, 6, 4]

        present_sbox = [0xc, 5, 6, 0xb, 9, 0, 0xa, 0xd, 3, 0xe, 0xf, 8, 4, 7, 1, 2]

        midori_sbox = [0xc, 0xa, 0xd, 3, 0xe, 0xb, 0xf, 7, 8, 9, 1, 5, 0, 2, 4, 6]

        # SBOX
        for i in range(8):
            variables = ["{0}[{1}:{1}]".format(s_in, 4*i + 3),
                         "{0}[{1}:{1}]".format(s_in, 4*i + 2),
                         "{0}[{1}:{1}]".format(s_in, 4*i + 1),
                         "{0}[{1}:{1}]".format(s_in, 4*i + 0),
                         "{0}[{1}:{1}]".format(p, 4*i + 3),
                         "{0}[{1}:{1}]".format(p, 4*i + 2),
                         "{0}[{1}:{1}]".format(p, 4*i + 1),
                         "{0}[{1}:{1}]".format(p, 4*i + 0),
                         "{0}[{1}:{1}]".format(w, 4*i + 3),
                         "{0}[{1}:{1}]".format(w, 4*i + 2),
                         "{0}[{1}:{1}]".format(w, 4*i + 1),
                         "{0}[{1}:{1}]".format(w, 4*i + 0)]
            command += stpcommands.add4bitSbox(present_sbox, variables)

        # PBOX+LBOX+Swap
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 0, s_out, 18)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 1, s_out,28)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 2, s_out,20)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 3, s_out,26)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 4, s_out,22)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 5, s_out,24)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 6, s_out,16)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 7, s_out,30)
        
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 8, s_out,19)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 9, s_out,29)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 10, s_out,21)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 11, s_out,27)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 12, s_out,23)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 13, s_out,25)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 14, s_out,17)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 15, s_out,31)

        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 16, s_out,2)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 17, s_out,12)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 18, s_out,4)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 19, s_out,10)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 20, s_out,6)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 21, s_out,8)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 22, s_out,0)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 23, s_out,14)

        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 24, s_out,3)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 25, s_out,13)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 26, s_out,5)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 27, s_out,11)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 28, s_out,7)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 29, s_out,9)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 30, s_out,1)
        command += "ASSERT({0}[{1}:{1}] = {2}[{3}:{3}]);\n".format(p, 31, s_out,15)

        stp_file.write(command)
        return
