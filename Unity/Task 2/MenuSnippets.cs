using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[InitializeOnLoad]
public class MenuSnippets
{
    // I want to keep trace of everything that i saved
    private static Dictionary<GameObject, GameObject> changedPrefabs;

    static MenuSnippets()
    {
        changedPrefabs = new Dictionary<GameObject, GameObject>();
        EditorApplication.playmodeStateChanged += OnPlayStateChanged;
    }

    static void OnPlayStateChanged()
    {
        if(!EditorApplication.isPlaying)
        {
            foreach(GameObject original in changedPrefabs.Keys)
            {
                // We can just go through the object
                // Change all component values with the ones that we have on the prefab
                GameObject ctx_copy_object = PrefabUtility.InstantiatePrefab(changedPrefabs[original]) as GameObject;

                Vector3 local_pos = ctx_copy_object.transform.position;
                PrefabUtility.UnpackPrefabInstance(ctx_copy_object, PrefabUnpackMode.Completely, InteractionMode.AutomatedAction);
                ctx_copy_object.transform.SetParent(original.transform.parent.transform);
                ctx_copy_object.transform.name = original.name;
                ctx_copy_object.transform.SetSiblingIndex(original.transform.GetSiblingIndex());
                ctx_copy_object.transform.localPosition = local_pos;

                // Method 2, more elegant, but still need work to go.
                //GameObject ctx_copy_object = PrefabUtility.InstantiatePrefab(prefab) as GameObject;
                //ctx_copy_object.hideFlags = HideFlags.HideInHierarchy;
                //Component[] components = ctx_copy_object.GetComponents(typeof(Component));

                //// Loop through all the components and copy from prefab to gameobject
                //for (int i = 0; i < components.Length; i++)
                //{
                //    System.Type typed = components[i].GetType();
                //    Component copy_comp = original.GetComponent(typed);
                //    copy_comp.GetCopyOf(components[i]);

                //    if(typed.Name == "MeshFilter")
                //    {
                //        Debug.Log("meshfolter");
                //    }
                //}

                UnityEngine.Object.DestroyImmediate(original);
                string prefab_path = AssetDatabase.GetAssetPath(changedPrefabs[original]);
                AssetDatabase.DeleteAsset(prefab_path);
            }
        }
    }

    [MenuItem("CONTEXT/Transform/Randomize Position")]
    private static void RandomizeTransform(MenuCommand cmd)
    {
        Transform trnobj = cmd.context as Transform;
        trnobj.position = new Vector3(Random.Range(-10,10f), Random.Range(-10, 10f), Random.Range(-10, 10f));

        // Modify this script to place an object into a proper or realistic place.
        // Raycast MUST be used.
    }

    [MenuItem("Assets/Process Texture")]
    private static void ProcessTexture(MenuCommand cmd)
    {
        //if the asset exists I want to do something.
        if(Selection.activeObject.GetType() == typeof(Texture2D))
        {
            Debug.Log("This is a texture");

            Texture2D tex = Selection.activeObject as Texture2D;
            string path = AssetDatabase.GetAssetPath(tex);

            // I need to import the texture, because its an asset.
            TextureImporter teximporter = AssetImporter.GetAtPath(path) as TextureImporter;
            {
                // Here we set the settings we want for our asset.
                teximporter.textureType = TextureImporterType.NormalMap;
                teximporter.filterMode = FilterMode.Trilinear;
                // Setup more settings here....
            }
            AssetDatabase.ImportAsset(path); // We are updating the asset we modified.
        }
    }

    [MenuItem("CONTEXT/Transform/Save GameObject")]
    private static void SaveGameObject(MenuCommand cmd)
    {
        // Get the actual gameobject from the context and save it as a prefab
        GameObject ctx_object = cmd.context as GameObject;
        GameObject prefab = PrefabUtility.CreatePrefab("Assets/copy_" + ctx_object.name + ".prefab", ctx_object);

        changedPrefabs.Add(ctx_object, prefab);
    }
}
