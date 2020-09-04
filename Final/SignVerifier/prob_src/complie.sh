TARGET_DIR="/home/signverifier"
JAVA_DIR="jdk-14.0.2"


if [ -d $TARGET_DIR ]
then
    echo "[+] clean ${TARGET_DIR}"
    sudo rm -rf $TARGET_DIR/*
    if [ $? -ne 0 ]
    then
        echo "[-] clean ${TARGET_DIR} fail"
        exit -1
    fi
else
    echo "[+] mkdir ${TARGET_DIR}"
    sudo mkdir $TARGET_DIR
    if [ $? -ne 0 ]
    then
        echo "[-] mkdir ${TARGET_DIR} fail"
        exit -1
    fi
fi

echo "[*] chmod and copy source to $TARGET_DIR"
sudo chmod 777 $TARGET_DIR
cp ./* $TARGET_DIR

echo "[*] cd into $TARGET_DIR"
cd $TARGET_DIR

echo "[*] unarchive jdk archive"
tar -xzvf openjdk-14.0.2_linux-x64_bin.tar.gz

echo "[*] make libSign.so"
gcc -o libSign.so -L./ SignVerifier.c aes.c -shared -fPIC -I $JAVA_DIR/include -I $JAVA_DIR/include/linux

echo "[*] make SignVerifier.jar"
echo "Main-Class: Runner" > manifest.txt
$JAVA_DIR/bin/javac Runner.java 
$JAVA_DIR/bin/jar -cvmf manifest.txt SignVerifier.jar *.class

echo "[*] Dump User class list with SignVerifier.jar"
$JAVA_DIR/bin/java -XX:DumpLoadedClassList=classes.lst -Djava.library.path=./ -jar SignVerifier.jar
echo "[*] Write Application CDS file with SignVerifier.jar"
$JAVA_DIR/bin/java -Xlog:class+path=info -Xshare:dump -XX:SharedClassListFile=classes.lst -XX:SharedArchiveFile=app-cds.jsa -Djava.library.path=./ --class-path SignVerifier.jar

echo "[*] make bin archive"
tar -zcvf bin.tar.gz libSign.so SignVerifier.jar app-cds.jsa