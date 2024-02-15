plugins {
    id("com.android.application")
}

android {
    namespace = "com.spawnland.tabbyEngineApp"
    compileSdk = 34

    defaultConfig {
        applicationId = "com.spawnland.tabbyEngineApp"
        minSdk = 23
        targetSdk = 34
<<<<<<< HEAD
        versionCode = 1
        versionName = "1.0"
=======
        versionCode = 13_02_2024_1
        versionName = "13.02.2024_1"
>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                cppFlags += "-std=c++17"
            }
        }
    }

    buildTypes {
        release {
<<<<<<< HEAD
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
        create("customDebugType") {
            isDebuggable = true

        }
=======
            isDebuggable = true
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    externalNativeBuild {
        cmake {
            path = file("../../App/CMakeLists.txt")
            version = "3.22.1"
        }
    }
<<<<<<< HEAD
=======

>>>>>>> 0be7a4d412b3627228f337d78beff91934d6db7f
    buildFeatures {
        viewBinding = true
    }
}

dependencies {

    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.9.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}