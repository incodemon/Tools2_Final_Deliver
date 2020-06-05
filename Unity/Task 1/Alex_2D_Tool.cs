using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.IO;


public class Alex_tool : EditorWindow
{
    public int width = 8;
    public float offset = 8.0f;
    public Vector2 gridSize;

    private List<string> instances;
    public string prefabsPath = "Assets/Prefabs";
    public string originalArtTextures = "Assets/Resources/Textures/Artist";
    public string convertedArtTextures = "Assets/Resources/Textures/Sprites";
    public Sprite mySprite;


    [MenuItem("Alex/2D Texture Manager")]
    static void Init()
    {
        Alex_tool window = (Alex_tool)EditorWindow.GetWindow(typeof(Alex_tool));
        window.Show();

    }

    void OnGUI()
    {
        //Canvas drawing elements
        //    prefabsPath = EditorGUILayout.TextField("Prefabs path", prefabsPath);
        originalArtTextures = EditorGUILayout.TextField("Art folder", originalArtTextures);
        convertedArtTextures = EditorGUILayout.TextField("Game textures folder", convertedArtTextures);

        if (GUILayout.Button("Convert Textures"))
        {
            //We delete the target folder's content and folder
            FileUtil.DeleteFileOrDirectory(convertedArtTextures + "/");
            //Here we create the new folder and copy data inside
            DirectoryCopy(originalArtTextures, convertedArtTextures, false);

     
            AssetDatabase.Refresh();
        }

        if (GUILayout.Button("Update Scene"))
        {
            instances = new List<string>();
            RetrievePrefabs(prefabsPath);
            UpdatePrefabs();
        }

    }

    void UpdatePrefabs()
    {
        foreach (string path in instances)
        {
            Object obj = AssetDatabase.LoadAssetAtPath(path, typeof(GameObject));
            GameObject contentsRoot = PrefabUtility.LoadPrefabContents(path);
            SpriteRenderer spriteR = contentsRoot.GetComponent<SpriteRenderer>();
            string objectname = obj.name + ".png";
            spriteR.sprite = (Sprite)AssetDatabase.LoadAssetAtPath("Assets/Resources/Textures/Sprites/" + objectname, typeof(Sprite));
            // contentsRoot.AddComponent<BoxCollider>();
            PrefabUtility.SaveAsPrefabAsset(contentsRoot, path);
        }
    }
    void SpawnPrefabs()
    {
        int x_gridpos = 0, y_gridpos = 0;
        foreach (string path in instances)
        {
            float pos_x = x_gridpos * (gridSize.x + offset);
            float pos_y = y_gridpos * (gridSize.y + offset);

            x_gridpos = (x_gridpos + 1) % width;
            y_gridpos += x_gridpos % width == 0 ? 1 : 0;

            Object obj = AssetDatabase.LoadAssetAtPath(path, typeof(GameObject));
            GameObject prefab = PrefabUtility.InstantiatePrefab(obj) as GameObject;
            prefab.transform.position = new Vector3(pos_x, 0, pos_y);
        }
    }
    void RetrievePrefabs(string path)
    {
        string[] prefabs = AssetDatabase.FindAssets("", new string[] { prefabsPath });
        string assetpath;
        int number = 0;

        foreach (string assetguid in prefabs)
        {
            number++;
            assetpath = AssetDatabase.GUIDToAssetPath(assetguid);
            if (AssetDatabase.IsValidFolder(assetpath))
            {
                RetrievePrefabs(assetpath);
                continue;
            }
            if (!instances.Contains(assetpath))
                instances.Add(assetpath);
        }
    }
    private static void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs)
    {
        // Get the subdirectories for the specified directory.
        DirectoryInfo dir = new DirectoryInfo(sourceDirName);

        if (!dir.Exists)
        {
            throw new DirectoryNotFoundException(
                "Source directory does not exist or could not be found: "
                + sourceDirName);
        }

        DirectoryInfo[] dirs = dir.GetDirectories();
        // If the destination directory doesn't exist, create it.
        if (!Directory.Exists(destDirName))
        {
            Directory.CreateDirectory(destDirName);
        }

        // Get the files in the directory and copy them to the new location.
        FileInfo[] files = dir.GetFiles();
        foreach (FileInfo file in files)
        {
            string temppath = Path.Combine(destDirName, file.Name);
            file.CopyTo(temppath, false);
        }

        // If copying subdirectories, copy them and their contents to new location.
        if (copySubDirs)
        {
            foreach (DirectoryInfo subdir in dirs)
            {
                string temppath = Path.Combine(destDirName, subdir.Name);
                DirectoryCopy(subdir.FullName, temppath, copySubDirs);
            }
        }
    }
}

public class SpriteProcessor : AssetPostprocessor
{
    public static TextureImporterType texturetype;

    void OnPostprocessTexture(Texture2D texture)
    {
        string lowerCaseAssetPath = assetPath.ToLower();
        bool isInSpritesDirectory = lowerCaseAssetPath.IndexOf("/sprites/") != -1;
        if (isInSpritesDirectory)
        {
            TextureImporter textureImporter = (TextureImporter)assetImporter;
            textureImporter.textureType = TextureImporterType.Sprite;
            textureImporter.spritePixelsPerUnit = 300;
            textureImporter.maxTextureSize = 1024;
            textureImporter.filterMode = FilterMode.Point;
            textureImporter.SaveAndReimport();
        }
    }
}

