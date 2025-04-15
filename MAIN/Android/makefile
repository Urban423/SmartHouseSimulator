Target=AndroidApp

CC = "C:\Program Files\android-ndk-r27\toolchains\llvm\prebuilt\windows-x86_64\bin\clang++"
sysroot = "C:\\Program Files\\android-ndk-r27\\toolchains\\llvm\\prebuilt\\windows-x86_64\\sysroot"
tar = aarch64-none-linux-android21
QFLAGS += -funwind-tables
QFLAGS += -D Android
QFLAGS +=  -stdlib=libc++ -fPIC -fstack-protector -Bdynamic -Bstatic -fuse-ld=lld

buildPath = ../../build/Android
bin = bin
folders = ${bin}
folders += ${buildPath}

pacakge = com.UrbanEngine.main
pacakgePath = com/UrbanEngine/main
AdnroidJarPath = ./lib/android.jar

SRC = ${wildcard ./jni/*.cpp}
OBJ = ${patsubst %.cpp, ${objDir}%.o, ${SRC}}

IFlags += -I ../../GraphicsEngine/
IFlags += -I ../../MAIN/
IFlags += -I ../../IOSystem/Android/
IFlags += -I ../../IOSystem/
IFlags += -I ../../Math/
IFlags += -I ../../App/
IFlags += -I ../../Logic/

CFlags += -L ./
CFlags += -lGLESv2
CFlags += -L ../../libs/
CFlags += -lApp
CFlags += -l Logic
CFlags += -lIOSystem
CFlags += -lGraphicsEngine
CFlags += -lMath
CFlags += -lz

buildApp: binFile javaCompiler CCompiler apkCreator
	adb install -r ${buildPath}/${Target}.apk
	adb shell am start --activity-clear-top -n ${pacakge}/.MainActivity


javaCompiler:
	aapt package -f -m -S ./res -J ${bin}/gen/ -M ./AndroidManifest.xml -I ${AdnroidJarPath}
	javac -h ./jni -classpath "C:/Program Files/Java/jdk-1.8/jre/lib/rt.jar" -cp ${AdnroidJarPath} -d ${bin}/obj ${bin}/gen/${pacakgePath}/R.java ./src/${pacakgePath}/*.java
	d8 --output=${bin}/apk/ ${bin}\obj\com\UrbanEngine\main\\*.class

CCompiler:
	${CC} --target=${tar} --sysroot=${sysroot} -c ./jni/AndroidFilter.cpp -o ./bin/AndroidFilter.o   ${QFLAGS} ${IFlags}
	${CC} --target=${tar} --sysroot=${sysroot} -shared -o ${bin}/apk/lib/arm64-v8a/libFilter.so  ./bin/AndroidFilter.o  ${QFLAGS} -lc++_static ${CFlags}
	llvm-strip --strip-unneeded ${bin}/apk/lib/arm64-v8a/libFilter.so

apkCreator:
	aapt package -f -M ./AndroidManifest.xml -S .\res -I ./lib/android.jar -F ${bin}/${Target}.unsigned.apk ${bin}\apk
	zipalign -f -p 4 ${bin}/${Target}.unsigned.apk ${bin}/${Target}.aligned.apk
	jarsigner -sigalg SHA1withRSA -digestalg SHA1 -keystore keystore.jks -storepass android -keypass android -signedjar ${buildPath}/${Target}.apk ${bin}/${Target}.aligned.apk androidkey

binFile: 
	if not exist ${bin} mkdir ${subst /,\, ${folders}}
	if not exist "${bin}\apk" mkdir ${bin}\apk
	if not exist "${bin}\obj" mkdir ${bin}\obj
	if not exist "${bin}\gen\com\UrbanEngine\main" mkdir ${bin}\gen\com\UrbanEngine\main
	if not exist "${bin}\apk\lib\arm64-v8a" mkdir ${bin}\apk\lib\arm64-v8a

Key:
	keytool -genkeypair -keystore keystore.jks -alias androidkey -validity 10000 -keyalg RSA -7 2048