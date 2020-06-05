using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

using UnityInternalEditor = UnityEditorInternal.InternalEditorUtility;

public class MVDUtils
{
    // All unity layers
    public static string[] RetrieveLayers()
    {
        List<string> layer_list = new List<string>();
        for (int i = 0; i <= UnityInternalEditor.layers.Length; i++) // There are 31 layers in unity, loop through them.
        {
            string layer_name = LayerMask.LayerToName(i);
            layer_list.Add(layer_name);
        }

        return layer_list.ToArray();
    }

    public static string[] RetrieveTags()
    {
        List<string> tag_list = new List<string>();
        for (int i = 0; i < UnityInternalEditor.tags.Length; i++)
        {
            tag_list.Add(UnityInternalEditor.tags[i]);
        }

        return tag_list.ToArray();
    }

    public static void RecursiveSetLayer(Transform trans, int layer)
    {
        trans.gameObject.layer = layer;

        if (trans.childCount > 0)
        {
            foreach (Transform t in trans)
                RecursiveSetLayer(t, layer);
        }
    }

    public static GameObject[] FindGameObjectsWithLayer(int layerIndex)
    {
        List<GameObject> finalObjects = new List<GameObject>();
        GameObject[] allObjects = GameObject.FindObjectsOfType(typeof(GameObject)) as GameObject[];

        for (var i = 0; i < allObjects.Length; i++)
        {
            if (allObjects[i].layer == layerIndex)
                finalObjects.Add(allObjects[i]);
        }

        return finalObjects.ToArray();
    }

    public static void ChangeAllMaterials(GameObject obj, Material mat)
    {
        Renderer rend = obj.GetComponent<Renderer>();

        if (rend)
            rend.sharedMaterial = mat;

        for (int i = 0; i < obj.transform.childCount; i++)
        {
            GameObject child = obj.transform.GetChild(i).gameObject;
            ChangeAllMaterials(child, mat);
        }
    }
}
