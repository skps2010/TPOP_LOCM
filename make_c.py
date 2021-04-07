if __name__ == "__main__":
    with open("Answer.cpp", "wb") as fp:
        stub = open("main_stub.cpp", "rb").read()
        fp.write(
            stub.replace(
                b"faceb00c",
                hex(len(stub) + 2).replace('0x',
                                           '').zfill(8).encode()))
        fp.write("/*".encode())
        fp.write(open("code/exe_bin.b64", "rb").read())
        fp.write("*/".encode())
